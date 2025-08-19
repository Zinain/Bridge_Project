use esp_idf_hal::delay::FreeRtos;
use esp_idf_hal::gpio::PinDriver;
use esp_idf_sys as _; // Bindings to ESP-IDF

fn main() -> anyhow::Result<()> {
    let peripherals = esp_idf_hal::peripherals::Peripherals::take().unwrap();

    // On many boards, LED is on GPIO2 — change if yours is different
    let mut pin1 = PinDriver::output(peripherals.pins.gpio4)?;
    let mut pin2 = PinDriver::output(peripherals.pins.gpio5)?;
    let mut pin3 = PinDriver::output(peripherals.pins.gpio10)?;
    let mut pin4 = PinDriver::output(peripherals.pins.gpio11)?;

    loop {
        pin4.set_high()?;           // LED on
        pin2.set_low()?;
        FreeRtos::delay_ms(5);  // 1 second
        pin3.set_high()?;
        pin1.set_low()?;
        FreeRtos::delay_ms(5);
        pin2.set_high()?;           // LED on
        pin4.set_low()?;
        FreeRtos::delay_ms(5);  // 1 second
        pin1.set_high()?;
        pin3.set_low()?;
        FreeRtos::delay_ms(5);
    }
}