#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial mySerial(10, 11);
TinyGPS gps;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
void setup()

{
  // Oploen serial communications and wait for port to open:
  lcd.begin(20, 4);
  lcd.clear();

  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(1000);

}

void loop() // run over and over
{
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000)
  {
    if (mySerial.available())

    {
      char c = mySerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c))
      {
        newdata = true;
        break;  // uncomment to print new data immediately!
      }
    }
  }

  if (newdata)
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump(gps);
    Serial.println("-------------");
    Serial.println();
    lcd.setCursor(15, 0);
    lcd.print(" ");
    delay(150);
    lcd.setCursor(15, 0);
    lcd.print("A");
    delay(150);

  }
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);
  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  lcd.setCursor(0, 0);
  lcd.print("UTC: ");
  lcd.print((static_cast<int>(hour)));
  lcd.print(":");
  lcd.print(static_cast<int>(minute));
  lcd.print(":");
  lcd.print(static_cast<int>(second));
  lcd.setCursor(0, 1);
  lcd.print(static_cast<int>(day));
  lcd.print("/");
  lcd.print(static_cast<int>(month));
  lcd.print("/"); 
  lcd.print(year);

  lcd.setCursor(0, 2);
  lcd.print("Lat:");
  lcd.print(flat, 5);
  lcd.setCursor(0, 3);
  lcd.print("Lon:");
  lcd.print(flon, 5);

  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
  Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/");
  Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour));  Serial.print(":"); //Serial.print("UTC +08:00 Malaysia");


  Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
  Serial.print("."); Serial.print(static_cast<int>(hundredths)); Serial.print(" UTC/GMT ");



  Serial.print("  Fix age: ");  Serial.print(age); Serial.println("ms.");

  Serial.print("Alt(cm): "); Serial.print(gps.altitude()); Serial.print(" Course(10^-2 deg): ");
  Serial.print(gps.course()); Serial.print(" Speed(10^-2 knots): "); Serial.println(gps.speed());
  Serial.print("Alt(float): "); printFloat(gps.f_altitude()); Serial.print(" Course(float): ");
  printFloat(gps.f_course()); Serial.println();
  Serial.print("Speed(knots): "); printFloat(gps.f_speed_knots()); Serial.print(" (mph): ");
  printFloat(gps.f_speed_mph());
  Serial.print(" (mps): "); printFloat(gps.f_speed_mps()); Serial.print(" (kmph): ");
  printFloat(gps.f_speed_kmph()); Serial.println();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
  Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
    Serial.print('-');
    number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i = 0; i < digits; ++i)
    rounding /= 10.0;

  number += rounding;
  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
