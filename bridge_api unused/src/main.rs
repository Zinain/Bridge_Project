use std::{thread::sleep, time::Duration, sync::{Mutex, Arc}, str::from_utf8, collections::HashMap};

use accel_stepper::{Driver, OperatingSystemClock};
use embedded_svc::{wifi::{Configuration, ClientConfiguration, AuthMethod}, http::Method::Post,http::Method::Get, io::Read};
use esp_idf_hal::{peripheral::Peripheral, prelude::Peripherals, gpio::PinDriver, ledc::{LedcTimerDriver, config::TimerConfig, LedcDriver}};
use esp_idf_svc::{eventloop::EspSystemEventLoop, nvs::{EspNvsPartition, NvsDefault, EspDefaultNvsPartition}, timer::{EspTimerService, Task, EspTaskTimerService}, wifi::{AsyncWifi, EspWifi}, ping::EspPing, http::server::EspHttpServer};
use esp_idf_sys as _; // If using the `binstart` feature of `esp-idf-sys`, always keep this module imported
use log::*;
use esp_idf_hal::units::*;

mod stepper;

const SSID: &str = env!("RUST_ESP32_STD_DEMO_WIFI_SSID");
const PASS: &str = env!("RUST_ESP32_STD_DEMO_WIFI_PASS");

fn main(){
    esp_idf_sys::link_pathes();
    esp_idf_svc::log::EspLogger::initialize_default();

    info!("Hello world!");

    let peripherals = Peripherals::take().unwrap();
    let sysloop = EspSystemEventLoop::take().unwrap();
    let timer_service = EspTaskTimerService::new().unwrap();
    let _wifi = wifi(peripherals.modem, sysloop, Some(EspDefaultNvsPartition::take().unwrap()), timer_service).unwrap();

}

pub fn wifi(
    modem: impl Peripheral<P = esp_idf_hal::modem::Modem> + 'static,
    sysloop: EspSystemEventLoop,
    nvs: Option<EspNvsPartition<NvsDefault>>,
    timer_service: EspTimerService<Task>,
) -> Result<AsyncWifi<EspWifi<'static>>>{
    use futures::executor::block_on;

    let mut wifi = AsyncWifi::wrap(
        EspWifi::new(modem, sysloop.clone(), nvs)?,
        sysloop,
        timer_service.clone(),
    )?:

    block_on(connect_wifi(&mut wifi))?;

    let ip_info = wifi.wifi().sta_netif().get_ip_info()?;

    EspPing::default().ping(ip_info.subnet.gateway, &embedded_svc::ping::Configuration::default())?;
    Ok(wifi)
}

async fn connect_wifi(wifi: &mut AsyncWifi<EspWifi<'static>>) -> anyhow::Result<()>{
    let wifi_configuration: Configuration = Configuration::Client(ClientConfiguration{
        ssid: SSID.into(),
        bssid: None,
        auth_method: AuthMethod::WPA2Personal,
        password: PASS.into(),
        channel: None,
    });

    wifi.set_configuration(&wifi_configuration)?;

    wifi.start().await?;
    info!("Wifi started");

    wifi.connect().await?;
    info!("Wifi connected");

    wifi.wait_netif_up().await?;
    info!("Wifi netif up");

    Ok(())
}


