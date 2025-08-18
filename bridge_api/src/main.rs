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
use anyhow::Result;
use esp_idf_svc::wifi::*;
use esp_idf_svc::eventloop::EspSystemEventLoop;
use esp_idf_svc::netif::*;
use esp_idf_svc::nvs::*;
use embedded_svc::wifi::*;
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
struct Request {
    command: String,
    value: i32,
}

#[derive(Serialize, Deserialize, Debug)]
struct Response {
    status: String,
    message: String,
}

fn connect_wifi(ssid: &str, password: &str) -> Result<()> {
    let sysloop = EspSystemEventLoop::take()?;
    let nvs = EspDefaultNvsPartition::take()?;
    let mut wifi = EspWifi::new(netif::EspNetifStack::new()?, sysloop.clone(), Some(nvs))?;

    wifi.set_configuration(&Configuration::Client(ClientConfiguration {
        ssid: ssid.into(),
        password: password.into(),
        ..Default::default()
    }))?;

    wifi.start()?;
    wifi.connect()?;
    wifi.wait_netif_up()?;

    println!("Connected to Wi-Fi: {}", ssid);
    Ok(())
}

fn main() -> Result<()> {
    esp_idf_sys::link_patches();
    println!("ESP32 Rust UDP JSON Client starting...");

    // Wi-Fi credentials
    let ssid = "YourWiFiSSID";
    let password = "YourWiFiPassword";
    connect_wifi(ssid, password)?;

    // UDP setup
    let server_addr = "192.168.1.100:8080"; // Replace with API server
    let local_addr = "0.0.0.0:12345";
    let socket = UdpSocket::bind(local_addr)?;
    println!("📡 UDP socket bound to {}", local_addr);

    // --- JSON Send ---
    let request = Request {
        command: "ping".to_string(),
        value: 42,
    };

    let json_str = serde_json::to_string(&request)?;
    socket.send_to(json_str.as_bytes(), server_addr)?;
    println!("Sent JSON: {}", json_str);

    // --- JSON Receive ---
    let mut buf = [0u8; 512];
    match socket.recv_from(&mut buf) {
        Ok((len, from)) => {
            let received = String::from_utf8_lossy(&buf[..len]);
            println!("Received from {}: {}", from, received);

            // Try to parse JSON
            match serde_json::from_str::<Response>(&received) {
                Ok(resp) => println!("Parsed Response: {:?}", resp),
                Err(e) => println!("Failed to parse JSON: {}", e),
            }
        }
        Err(e) => {
            println!("Failed to receive: {}", e);
        }
    }

    Ok(())
}
