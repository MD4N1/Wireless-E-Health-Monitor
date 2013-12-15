/*
Tiap pembacaan sensor harus ditulis dalam 1 baris keluaran serial. Baris harus diakhiri dengan '\n' dan harus mengikuti format berikut:

<Nilai Pengukuran>;<nama paket sensor>;<nama sensor>;<Tipe pengukuran>;<tipe pendek pengukuran>;<Nama satuan>;<Simbol/singkatan satuan>;<T1>;<T2>;<T3>;<T4>;<T5>

Nama sensor harus berbeda untuk tiap sensor.

T1..T5 ambang batas intejer yang memandu bagaimana angka-angkanya ditampilkan -
- kurang dari T1 - sangat rendah / tidak akan ditampilkan
- T1 < Nilai < T2 - rendah / hijau
- T2 < Nilai < T3 - sedang / kuning
- T3 < Nilai < T4 - tinggi / oranye
- T4 < Nilai < T5 - sangat tinggi / merah
- > T5 - paling tinggi / tidak akan ditampilkan
*/
#include <Arduino.h>
#include <Usb.h>
#include <SPP.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Kabel data disambungkan ke pin 2 Arduino
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

USB Usb;
BTD Btd(&Usb); // Buat Bluetooth Dongle
SPP SerialBT(&Btd,"WSN","1234"); //Nama Devais Bluetooh Dongle dan PIN nya

float maxv, CO, NO2, enerji, tegangan;
int cahaya, humi, cel, circ = 5, heat = 6, interval = 2500;
unsigned long previousMillis = 0;



float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void GetHumi()
{
  float val0 = analogRead(A0);
  float maxv = (3.27-(0.006706*cel));
  humi = ((((val0/1023)*5)-0.8)/maxv)*100;
}

void GetCO()
{
  digitalWrite(circ, LOW);
  analogWrite(heat, 245);
  delay(14);
  analogWrite(heat, 0);
  delay(981);
  digitalWrite(circ, HIGH);
  delay(3);
  float val1 = analogRead(A1);
  CO = map(val1, 0 , 1023, 0, 100);
}

void GetVoltage()
{
  float val0 = analogRead(A2);
  tegangan = map(val0, 0 , 1023, 0, 5000);

}

void GetTemp()
{
  sensor.requestTemperatures();
  cel = sensor.getTempCByIndex(0);

}

void GetNO2()
{
  float val3 = analogRead(A3);
  NO2 = map(val3, 1023, 0, 0, 100);
}

void GetPower()
{
  float val3= analogRead(A4);
  enerji=map(val3, 0, 1023, 0, 100);
}

void GetLightIntensity()
{
  float val3= analogRead(A5);
  cahaya=map(val3, 0, 1023, 0, 500);
}
void setup()
{
  pinMode(circ, OUTPUT);
  pinMode(heat, OUTPUT);

  // Start up the library
  sensor.begin();
  if (Usb.Init() == -1)
  {
    while (1); //halt
  }
}


void loop()
{
  Usb.Task();
  if (SerialBT.connected)
  {
        unsigned long currentMillis = millis();
        if ((unsigned long)(currentMillis - previousMillis) >= interval)
        {
            //Panggil fungsi-fungsi sensor
            GetHumi();
            GetCO();
            GetVoltage();
            GetTemp();
            GetNO2();
            //GetPower();
            GetLightIntensity();

            //Tampilkan kelembaban udara
            SerialBT.print(humi);
            SerialBT.print(";InsertSensorPackageName;HIH4030;Kelembaban udara;RH;persen;%;0;25;50;75;100");
            SerialBT.print("\n");

            //Tampilkan sensor gas CO
            SerialBT.print(CO);
            SerialBT.print(";InsertSensorPackageName;TGS2442;Gas CO;ppm;indikator respon;RI;0;25;50;75;100");
            SerialBT.print("\n");

            //Tampilkan tegangan
            SerialBT.print(tegangan);
            SerialBT.print(";InsertSensorPackageName;DAC;Tegangan;mVolt;Tegangan;mV;0;1000;2000;3000;4000");
            SerialBT.print("\n");

            //Tampilkan sensor temperatur
            SerialBT.print(cel);
            SerialBT.print(";InsertSensorPackageName;DS1820;Temperatur;C;derajat Celsius;C;0;20;40;60;80");
            SerialBT.print("\n");

            //Tampilkan gas NO2
            SerialBT.print(NO2);
            SerialBT.print(";InsertSensorPackageName;MiCS-2710;Gas N02;NO2;indikator respon;RI;0;25;50;75;100");
            SerialBT.print("\n");

            /*
            //Tampilkan Enerji Listrik
            SerialBT.print(enerji);
            SerialBT.print(";InsertSensorPackageName;Breakout Power;Enerji Listrik;enerji;kwh;kwh;0;25;50;75;100");
            SerialBT.print("\n");
            */
            //Tampilkan intensitas cahaya
            SerialBT.print(cahaya);
            SerialBT.print(";InsertSensorPackageName;LDR;Intensitas Cahaya;cahaya;Lumen;Lux;0;50;100;200;400");
            SerialBT.print("\n");
            previousMillis = currentMillis;
        }
    }
}

