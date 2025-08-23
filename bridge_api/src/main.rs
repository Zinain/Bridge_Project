/*use esp_idf_hal::prelude::*;
use esp_idf_hal::gpio::{PinDriver, Output};
use std::thread::sleep;
use std::time::Duration;

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
}*/ //this is a working test for a motor
use std::net::TcpStream;
use std::io::{Read, Write};
use std::time::Duration;
use serde_json::json;

fn main() -> anyhow::Result<()> {
    // Server IP (replace with your PC's IP where Python server is running)
    let server_addr = "192.168.1.50:5000";

    // Connect to server
    let mut stream = TcpStream::connect(server_addr)?;
    stream.set_read_timeout(Some(Duration::from_secs(5)))?;
    println!("Connected to server {}", server_addr);

    // Example JSON message
    let message = json!({
        "type": "COMMAND",
        "command": "OPENB",
        "timestamp": chrono::Utc::now().to_rfc3339(),
    });

    // Send JSON
    let json_str = message.to_string();
    stream.write_all(json_str.as_bytes())?;
    println!("Sent: {}", json_str);

    // Receive response
    let mut buf = [0u8; 4096];
    let size = stream.read(&mut buf)?;
    let response = String::from_utf8_lossy(&buf[..size]);
    println!("Received: {}", response);

    Ok(())
}


