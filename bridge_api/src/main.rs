use esp_idf_hal::prelude::*;
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
}