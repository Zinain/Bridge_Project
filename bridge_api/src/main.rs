/*use esp_idf_hal::prelude::*;
use esp_idf_hal::gpio::{PinDriver, Output};
use std::thread::sleep;
use std::time::Duration;

fn main() -> anyhow::Result<()> {
    esp_idf_sys::link_patches(); // required for ESP-IDF runtime

    let peripherals = Peripherals::take().unwrap();
    let mut led = PinDriver::output(peripherals.pins.gpio48)?;

    loop {
        led.set_high()?;
        sleep(Duration::from_millis(500));
        led.set_low()?;
        sleep(Duration::from_millis(500));
    }
}*/ //this is a working test for a motor
use std::net::UdpSocket;
use std::time::Duration;
use heapless::String;
use core::convert::TryFrom;

use esp_idf_svc::eventloop::EspSystemEventLoop;
use esp_idf_svc::nvs::EspDefaultNvsPartition;
use esp_idf_svc::wifi::{BlockingWifi, EspWifi};
use esp_idf_hal::peripherals::Peripherals;

fn main() -> anyhow::Result<()> {
    // Initialize peripherals
    let peripherals = Peripherals::take().unwrap();
    let modem = peripherals.modem;


    // Take system event loop
    let sysloop = EspSystemEventLoop::take()?;

    // Setup Wi-Fi
    let nvs_partition = EspDefaultNvsPartition::take()?;
    let mut wifi = EspWifi::new(modem, sysloop.clone(), Some(nvs_partition))?;

    let mut wifi = BlockingWifi::wrap(&mut wifi, sysloop)?;

    let ssid: String<32> = String::try_from("check").unwrap();
    let pass: String<64> = String::try_from("123456789").unwrap();

    wifi.set_configuration(&esp_idf_svc::wifi::Configuration::Client(
        esp_idf_svc::wifi::ClientConfiguration {
            ssid,
            password: pass,
            ..Default::default()
        },
    ))?;

    wifi.start()?;
    wifi.connect()?;

    while !wifi.is_connected()? {
        std::thread::sleep(Duration::from_millis(100));
    }

    println!("Connected to Wi-Fi!");
    println!("IP info: {:?}", wifi.wifi().sta_netif().get_ip_info()?.ip);

    // UDP example
    let socket = UdpSocket::bind("192.168.1.100:12345")?;
    socket.set_read_timeout(Some(Duration::from_secs(5)))?;

    let mut buf = [0u8; 512];
    loop {
        match socket.recv_from(&mut buf) {
            Ok((size, src)) => {
                let msg = core::str::from_utf8(&buf[..size]).unwrap();
                println!("Received from {}: {}", src, msg);

                // Echo JSON back
                socket.send_to(msg.as_bytes(), src)?;
            }
            Err(e) => {
                println!("Timeout or error: {:?}", e);
            }
        }
    }
}
