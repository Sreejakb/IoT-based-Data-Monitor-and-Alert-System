#include <boltiot.h>
#include <DHT.h>
#include <LCD_I2C.h>
#include <Wire.h>
#include <MQ135.h>
#include <MQ2.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FastBot.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#ifndef API_KEY
#define API_KEY "<Bolt IoT WiFi Module API Key>"
#endif
#ifndef DEVICE_ID
#define DEVICE_ID "<Bolt Device ID>"
#endif

#define DHTPIN 14
#define DHTTYPE DHT22
#define WIFI_SSID "<Wifi SSID>"
#define WIFI_PASS "<Wifi Password>"
#define BOT_TOKEN "<Telegram Bot Token>"
#define MUX_A D4 //Multiplexer Pin D4
#define flame D0
#define dataPin 12   //Outputs the byte to transfer
#define loadPin 13   //Controls the internal transference of data in SN74HC595 internal registers
#define clockPin 15  //Generates the clock signal to control the transference of data
#define Buzzer D3

int changeMux(int a) { // Function for Multiplexer
  digitalWrite(MUX_A, a);
  return A0;
}

LCD_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(changeMux(HIGH));
MQ2 mq2(changeMux(LOW));
WiFiClientSecure client;
FastBot bot(BOT_TOKEN); //Telegram bot

int f, T, H, A, L, C, S, FL, ftime, ttime, afh, afm, ath, atm;
int ch = 0;
float Temperature, Humidity, PPM, lpg, co, smoke;
String fl, readings, threshold, temperature, humidity, airquality, Lpg, carbon_monoxide, smk, webserver, wifi, r, link, details, fdate, fhr, fmn, tdate, thr, tmn, afhr, afmn, athr, atmn;
String tlimit = "40";  //Temperature Limit
String hlimit = "80"; //Humidity Limit
String alimit = "6000"; //Air Quality Limit
String llimit = "3"; //LPG Limit
String slimit = "5"; //Carbon Monoxide Limit
String climit = "5"; //Smoke Limit
String CHAT_ID = "<Your Telegram Chat ID>";
const char* PARAM_INPUT_1 = "t_limit";
const char* PARAM_INPUT_2 = "h_limit";
const char* PARAM_INPUT_3 = "a_limit";
const char* PARAM_INPUT_4 = "l_limit";
const char* PARAM_INPUT_5 = "c_limit";
const char* PARAM_INPUT_6 = "s_limit";
const char* PARAM_INPUT_7 = "ID";

String pushData(String *data) {  // Push data to Bolt IoT Cloud
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  PPM = mq135_sensor.getCorrectedPPM(Temperature, Humidity);
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  return String(Temperature) + "," + String(Humidity) + "," + String(PPM) + "," + String(lpg) + "," + String(co) + "," + String(smoke);
}

// Webserver Code
const char index_html[] PROGMEM = R"rawliteral(
 <!DOCTYPE HTML>
  <html><head>
  <title>Data Monitor and Alert System</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css"> 
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css" integrity="sha512-9usAa10IRO0HhonpyAIVpjrylPvoDwiPUiKdWk5t3PyolY1cOd4DSE0Ga+ri4AuTroPR5aQvXU9xC6qOPnzFeg==" crossorigin="anonymous" referrerpolicy="no-referrer">   
  <link rel="stylesheet" href="https://unpkg.com/aos@next/dist/aos.css" />
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
  <style>
  body {
   background: #f1f5f7;
   text-align: center;
  }
  .rainbow {
  font-family:arial black;
  background-image: 
    linear-gradient(to right, red,orange,yellow,green,blue,indigo,violet, red); 
  -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;  
  animation: fade 1000ms ease-in-out forwards , rainbow-animation 45s linear infinite;  
  }
  @keyframes rainbow-animation {
    to {
        background-position: 4500vh;
    }
  }
   .shadow {
  text-shadow:
    0 0 5px #ff0,
    0 0 10px #d9f4ff,
    0 0 20px #78ffbb,
    0 0 40px #d9fffb,
    0 0 80px #aae3f2,
    0 0 90px #bfe9f5,
    0 0 100px #d9f7ff,
    0 0 150px #fff;
  }
  .glowing {
      -webkit-animation: glowing 1.4s ease-in-out infinite alternate;
      -moz-animation: glowing 1.4s ease-in-out infinite alternate;
      animation: glowing 1.4s ease-in-out infinite alternate;
   }
   @-webkit-keyframes glowing{
      from {
         font-family:arial black;
  color: #00ff00;
  text-shadow:
    0 0 5px #4d048c,
    0 0 10px #060a7d,
    0 0 20px #3437e3,
    0 0 40px #5d5fe8,
    0 0 80px #5fb4ed,
    0 0 90px #77beed,
    0 0 100px #9bdaf2,
    0 0 150px #c1e6f5;
      }
      to {
    font-family:arial black;
    color: #00ff00;
    text-shadow:
    0 0 5px #83d4f7,
    0 0 10px #4346e0,
    0 0 20px #6769e6,
    0 0 40px #7879eb,
    0 0 80px #7ac1f0,
    0 0 90px #95cbf0,
    0 0 100px #aee0f2,
    0 0 150px #e1f1f7;
   }
  }
  .text-animation {
  opacity:0;
  }
  .text-animation span {
  top:50px;
  left:50px;
  opacity:0;
  animation:fade 400ms ease-in-out forwards;
  }
  @keyframes fade {
  0% {
    top:50px;
    left:50px;
    filter:blur(50px);
    opacity:0;
  }
  50% {
    filter:blur(10px);
    opacity:0.9;
  }
  100% {
    top:0px;
    left:0px;
    filter:blur(0px);
    opacity:1;
  }  
  }
  .border {
  border: 3px solid red;
  border-radius: 20px;
  padding: 15px;
  margin: 4px;
  }
  p{font-size:22px; font-family:arial black;}
  form{font-size:15px;}
  input[type="text"]
  {
    background: transparent;
    border: 3px dotted red;
    border-radius: 20px;
  }
  input[type="submit"]
  {
    border: 1.8px solid red;
    border-radius: 20px;
    outline:none;
  }
  .button {
  background-color: #07e371;
  border: none;
  color: white;
  padding: 10px 10px;
  text-align: center;
  display: inline-block;
  border-radius: 10px;
  font-size: 14px;
  margin: 4px 2px;
  cursor: pointer;
  -webkit-transition-duration: 0.2s; 
  transition-duration: 0.2s;
  }
  .button1:hover {
  box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);
  border-radius: 12px;
  font-size: 16px;
   }
  </style>
   </head><body class="border">  
    <h1 class="rainbow" style="font-size:58px;" data-aos="zoom-in" data-aos-once="false" data-aos-delay="200">Data Monitor and Alert System </h1>
    <h2 class="glowing" style="font-size:48px;" data-aos="flip-down" data-aos-once="false" data-aos-duration="1500">Data Monitor</h2>
    <div data-aos="zoom-in" data-aos-offset="150" data-aos-once="false" data-aos-easing="ease-in-cubic">
    <p><i class="fa fa-thermometer-quarter" style="color:#0000ff;font-size:50px;"></i> Temperature: <span id="temp"> %TEMPERATURE% </span>&deg;C </p>
    <p><i class="fa fa-cloud" style="color: #e4e8ed;font-size:50px;"></i> Humidity: <span id="hum"> %HUMIDITY% </span>&#37</p>
    <p><i class="fa-solid fa-mask-face" style="color:#00ff00;font-size:55px;"></i> Air Quality: <span id="airq"> %AIRQ% </span> ppm </p>
    <p><i class="fas fa-gas-pump" style="color: #ff9900;font-size:47px;"></i> LPG: <span id="liq"> %LIQ% </span> ppm </p>
    <p><i class="fas fa-biohazard" style="color: #dbd221;font-size:35px;"></i> Carbon Monoxide: <span id="cbm"> %CBM% </span> ppm </p>
    <p><i class="fas fa-smog" style="color: #e4e8ed;font-size:50px;"></i> Smoke: <span id="smok"> %SMOK% </span> ppm </p>
    <p><i class="fa-solid fa-fire-flame-curved" style="color: red;font-size:47px;"></i> Fire: <span id="fla"> %FLA% </span> </p><br>
    </div>
    <h3 style="color:#ff0000;font-size:24px;" class="shadow , text-animation" data-aos="slide-right" data-aos-once="false" data-aos-duration="1200">Set the values for the below parameters for Alert:</h3>
    <form action="/get" data-aos="slide-left" data-aos-once="true">
    <i class="fa fa-thermometer-4" style="color: red"></i> Temperature Threshold (in &deg;C): <input type="text" name="t_limit" maxlength="6" required="">
    <input type="submit" value="Submit">
    </form><br>
    <form action="/get" data-aos="slide-right" data-aos-once="true">
    <i class="fa fa-cloud" style="color: #372170"></i> Humidity Threshold (in %): <input type="text" name="h_limit" maxlength="6" required="">
    <input type="submit" value="Submit">
    </form><br>
    <form action="/get" data-aos="slide-left" data-aos-once="true">
    <i class="fa-solid fa-mask-face" style="color:#ff0000"></i> Air Quality Threshold (in ppm): <input type="text" name="a_limit" maxlength="8" required="">
    <input type="submit" value="Submit">
    </form><br>
    <form action="/get" data-aos="slide-right" data-aos-once="true">
    <i class="fas fa-gas-pump" style="color: #ff0000"></i> LPG Threshold (in ppm): <input type="text" name="l_limit" maxlength="5" required="">
    <input type="submit" value="Submit">
    </form><br>
    <form action="/get" data-aos="slide-left" data-aos-once="true">
    <i class="fas fa-biohazard" style="color: #ff0000"></i> Carbon Monoxide Threshold (in ppm): <input type="text" name="c_limit" maxlength="5" required="">
    <input type="submit" value="Submit"> 
    </form><br>
    <form action="/get" data-aos="slide-right" data-aos-once="true">
    <i class="fas fa-smog" style="color: #7c7c80"></i> Smoke Threshold (in ppm): <input type="text" name="s_limit" maxlength="5" required="">
    <input type="submit" value="Submit">
    </form><br>
    <form action="/get" data-aos="slide-left" data-aos-once="true">
    <i class="fa fa-telegram" style="color: #1811d6"></i> Telegram Chat ID (add multiple ids using ","): <input type="text" name="ID" required="">
    <input type="submit" value="Submit">
    </form><br><br>
    <h3 data-aos="slide-down" data-aos-once="false">For viewing historic data <i class="fa fa-database"></i> and check data visualization <i class="fa fa-line-chart"></i><br>
    Click on the link below <i class="fas fa-hand-point-down"></i> </h3>
    <a href="<Bolt IoT Cloud Link>"><button class="button button1"><i class="fa fa-cloud-upload"></i> Go to Bolt IoT Cloud</button></a>
    <script>
    setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temp").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/temp", true);
    xhttp.send();
    }, 2600 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("hum").innerHTML = this.responseText;
   }
   };
    xhttp.open("GET", "/hum", true);
    xhttp.send();
   }, 2400 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("airq").innerHTML = this.responseText;
   }
   };
    xhttp.open("GET", "/airq", true);
    xhttp.send();
   }, 2200 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("liq").innerHTML = this.responseText;
    }
   };
    xhttp.open("GET", "/liq", true);
    xhttp.send();
   }, 2000 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("cbm").innerHTML = this.responseText;
    }
   };
    xhttp.open("GET", "/cbm", true);
    xhttp.send();
   }, 1800 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("smok").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/smok", true);
    xhttp.send();
   }, 1600 ) ;
   setInterval(function () {
   var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("fla").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/fla", true);
    xhttp.send();
    }, 1400 ) ;
   </script><script>
   var wrapper = document.getElementsByClassName("text-animation")[0];
   wrapper.style.opacity="1";
   wrapper.innerHTML = wrapper.textContent.replace(/./g,"<span>$&</span>");
   var spans = wrapper.getElementsByTagName("span");
   for(var i=0;i<spans.length;i++){
   spans[i].style.animationDelay = i*150+"ms";
   }  
   </script>
   <script src="https://unpkg.com/aos@next/dist/aos.js"></script>
   <script>
   AOS.init();
   </script></body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Error 404: Not found on this webserver");
}

AsyncWebServer server(80);  

String processor(const String& var) {
  if (var == "TEMPERATURE") {
    return String(Temperature);
  }
  else if (var == "HUMIDITY") {
    return String(Humidity);
  }
  else if (var == "AIRQ") {
    return String(PPM);
  }
  else if (var == "LIQ") {
    return String(lpg);
  }
  else if (var == "CBM") {
    return String(co);
  }
  else if (var == "SMOK") {
    return String(smoke);
  }
  else if (var == "FLA") {
    return fl;
  }
  else if (var == "t_limit") {
    return tlimit;
  }
  else if (var == "h_limit") {
    return hlimit;
  }
  else if (var == "a_limit") {
    return alimit;
  }
  else if (var == "l_limit") {
    return llimit;
  }
  else if (var == "c_limit") {
    return climit;
  }
  else if (var == "s_limit") {
    return slimit;
  }
  else if (var == "ID") {
    return CHAT_ID;
  }
  return String();
}

//Telegram Message handler
void newMsg(FB_msg& msg) {
  r = msg.text.substring(0, 3);
  if (msg.text == "/start"  || msg.text == "/menu" || msg.text == "/back") {
    bot.sendMessage("Welcome to Alert Bot (Designed By Sreejak)\n <b><u>Menu:</u></b> \n1) Reading of individual sensors: /reading \n2) Readings of all the sensors: /readings \n3) View and download readings: /view \n4) Check Threshold Limit: /threshold  \n5) Change Threshold values: /change \n6) Change Telegram Chat ID: /telegram");
  }
  if (msg.text == "/reading") {
    bot.sendMessage("Reading of individual sensors: \n1) Temperature Reading: /temperature \n2) Humidity Reading: /humidity \n3) Air Quality Reading: /airquality \n4) LPG Reading: /lpg \n5) Carbon Monoxide Reading: /co \n6) Smoke Reading: /smoke \n7) Go to main menu: /back");
  }
  if (msg.text == "/readings") {
    bot.sendMessage(readings);
  }
  if (msg.text == "/view") {
    bot.sendMessage("View and download readings: \n1) View on the webserver: /webserver \n2) View and check data visualization: /cloud \n3) Download all the previous readings in CSV format: /download \n4) Download previous readings during specific duration in CSV format: /downloads \n5) Go to main menu: /back");
  }
  if (msg.text == "/threshold") {
    bot.sendMessage(threshold);
  }
  if (msg.text == "/change") {
    bot.sendMessage("Change Threshold Values: \n1) Change using webserver: /webserver \n2) Change Temperature threshold: /tchange \n3) Change Humidity threshold: /hchange \n4) Change Air Quality threshold: /achange \n5) Change LPG threshold: /lchange \n6) Change Carbon Monoxide threshold: /cchange \n7) Change smoke threshold: /schange \n8) Go to main menu: /back");
  }
  if (msg.text == "/temperature") {
    bot.sendMessage(temperature);
  }
  if (msg.text == "/humidity") {
    bot.sendMessage(humidity);
  }
  if (msg.text == "/airquality") {
    bot.sendMessage(airquality);
  }
  if (msg.text == "/lpg") {
    bot.sendMessage(Lpg);
  }
  if (msg.text == "/co") {
    bot.sendMessage(carbon_monoxide);
  }
  if (msg.text == "/smoke") {
    bot.sendMessage(smk);
  }
  if (msg.text == "/cloud") {
    bot.sendMessage("To view previous data and check data visualization, click on the link: <Bolt IoT Cloud Link>");
  }
  if (msg.text == "/download") {
    bot.sendMessage("Click on the link to download all the previous readings in CSV format: <Bolt IoT Cloud Download Button Link>");
  }
  if (msg.text == "/downloads") {
    bot.sendMessage("Enter the details: From Date, From Time, To Date, To Time (in the format dat:YYYY-MM-DD,HH:MM,YYYY-MM-DD,HH:MM)(Time in 24 hr format)(use no spacing while entering)");
  }
  if (r == "dat") {
    details = msg.text.substring(4);
    bot.sendMessage("Type /getlink to get the link");
  }
  if (msg.text == "/getlink") {
    bot.sendMessage(link);
  }
  if (msg.text == "/webserver") {
    bot.sendMessage(webserver);
  }
  if (msg.text == "/tchange") {
    bot.sendMessage("Enter the new threshold value for Temperature in this format(tcf:value):(eg:tcf:10)");
  }
  if (r == "tcf") {
    tlimit = msg.text.substring(4);
    bot.sendMessage(String("Temperature threshold value has been changed to ") + msg.text.substring(4) + "°C.");
  }
  if (msg.text == "/hchange") {
    bot.sendMessage("Enter the new threshold value for Humidity in this format(hcf:value):(eg:hcf:40)");
  }
   if (r == "hcf") {
    hlimit = msg.text.substring(4);
    bot.sendMessage(String("Humidity threshold value has been changed to ") + msg.text.substring(4) + "%.");
  }
  if (msg.text == "/achange") {
    bot.sendMessage("Enter the new threshold value for Air Quality in this format(acf:value):(eg:acf:500)");
  }
  if (r == "acf") {
   alimit = msg.text.substring(4);
   bot.sendMessage(String("Air Quality threshold value has been changed to ") + msg.text.substring(4) + " ppm.");
  }
  if (msg.text == "/lchange") {
   bot.sendMessage("Enter the new threshold value for LPG in this format(lcf:value):(eg:lcf:2)");
  }
  if (r == "lcf") {
   llimit = msg.text.substring(4);
   bot.sendMessage(String("LPG threshold value has been changed to ") + msg.text.substring(4) + " ppm.");
  }
  if (msg.text == "/cchange") {
    bot.sendMessage("Enter the new threshold value for Carbon Monoxide in this format(ccf:value):(eg:ccf:1)");
  }
  if (r == "ccf") {
   climit = msg.text.substring(4);
   bot.sendMessage(String("Carbon Monoxide threshold value has been changed to ") + msg.text.substring(4) + " ppm.");
  }
  if (msg.text == "/schange") {
    bot.sendMessage("Enter the new threshold value for Smoke in this format(scf:value):(eg:scf:1)");
  }
  if (r == "scf") {
   slimit = msg.text.substring(4);
   bot.sendMessage(String("Smoke threshold value has been changed to ") + msg.text.substring(4) + " ppm.");
  }
  if (msg.text == "/telegram") {
   bot.sendMessage("Enter Telegram Chat ID to add or change in this format(idc:value)(Add Multiple IDs using ','):(eg:idc:xxxxx,xxxxx)");
  }
  if (r == "idc") {
   CHAT_ID = msg.text.substring(4);
   bot.sendMessage(String("Telegram Chat ID has been changed to ") + msg.text.substring(4) + ".");
  }
}

void setup() {
  boltiot.begin(Serial);
  Serial.begin(9600);
  pinMode(MUX_A, OUTPUT);
  pinMode(flame, INPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  lcd.begin(); //initialize the lcd
  lcd.backlight(); //open the backlight
  dht.begin();
  mq2.begin();
  boltiot.setCommandString("RD\r", pushData);
  boltiot.setCommandString("GetData", pushData);
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, ch); //change led according to the Alert Number
  digitalWrite(loadPin, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("  Data Monitor and ");
  lcd.setCursor(0, 1);
  lcd.print("    Alert System");
  lcd.setCursor(0, 2);
  lcd.print("      Designed By:");
  lcd.setCursor(0, 3);
  lcd.print("     Sreejak Bhuniya");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to Wifi:");
  lcd.setCursor(0, 1);
  lcd.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    lcd.print(".");
    delay(500);
  }
  lcd.setCursor(0, 2);
  lcd.print("WiFi connected ");
  lcd.setCursor(0, 3);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  wifi = WiFi.localIP().toString();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(Temperature).c_str());
  });
  server.on("/hum", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(Humidity).c_str());
  });
  server.on("/airq", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(PPM).c_str());
  });
  server.on("/liq", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(lpg).c_str());
  });
  server.on("/cbm", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(co).c_str());
  });
  server.on("/smok", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(smoke).c_str());
  });
  server.on("/fla", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", fl.c_str());
  });
  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    if (request->hasParam(PARAM_INPUT_1)) {
      tlimit = request->getParam(PARAM_INPUT_1)->value();
      request->send(200, "text/html", "Temperature Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_2)) {
      hlimit = request->getParam(PARAM_INPUT_2)->value();
      request->send(200, "text/html", "Humidity Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_3)) {
      alimit = request->getParam(PARAM_INPUT_3)->value();
      request->send(200, "text/html", "Air Quality Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_4)) {
      llimit = request->getParam(PARAM_INPUT_4)->value();
      request->send(200, "text/html", "LPG Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_5)) {
      climit = request->getParam(PARAM_INPUT_5)->value();
      request->send(200, "text/html", "Carbon Monoxide Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_6)) {
      slimit = request->getParam(PARAM_INPUT_6)->value();
      request->send(200, "text/html", "Smoke Threshold limit has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
    if (request->hasParam(PARAM_INPUT_7)) {
      CHAT_ID = request->getParam(PARAM_INPUT_7)->value();
      request->send(200, "text/html", "Telegram Chat ID has been changed temporarily.<br><a href=\"/\">Return to Home Page</a>");
    }
  });
  server.onNotFound(notFound);
  server.begin();
  bot.setChatID(CHAT_ID);
  bot.setTextMode(FB_HTML);
  bot.sendMessage("⚠<b>Alert System has Started</b>⚠");
  bot.attach(newMsg);
  delay(2000);
  lcd.clear();
}

void loop() {
  String tmes ="", hmes = "", ames = "", lmes = "", cmes = "", smes = "", fmes = "";
  bot.tick();
  boltiot.handleCommand();
  float* values = mq2.read(false);
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  PPM = mq135_sensor.getCorrectedPPM(Temperature, Humidity);
  f = digitalRead(flame);
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.print(Temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Hmd:");
  lcd.print(int(Humidity));
  lcd.print("% ");
  lcd.print("AirQ:");
  lcd.print(int(PPM));
  lcd.print("ppm");
  lcd.setCursor(0, 2);
  lcd.print("LPG:");
  lcd.print(int(lpg));
  lcd.print("ppm ");
  lcd.print("CO:");
  lcd.print(co);
  lcd.print("ppm");
  lcd.setCursor(0, 3);
  lcd.print("Smk:");
  lcd.print(smoke);
  lcd.print("ppm ");
  lcd.print("Fire:");
  if (f == 1) {
    FL = 0;
    lcd.print("No");
    fl = "Not Detected";
  }
  else {
    FL = 1;
    lcd.print("Yes");
    fl = "Detected";
    fmes = "🔥<b>Fire</b>🔥 is dectected.\n";
  }
  readings = String("Current readings of all the sensors: \n1) Temperature: ") + Temperature + "°C \n2) Humidity: " + Humidity + "% \n3) Air Quality: " + PPM + " ppm \n4) LPG: " + lpg + " ppm \n5) Carbon Monoxide: " + co + " ppm \n6) Smoke: " + smoke + " ppm  \n7) Fire: " + fl;
  threshold = String("Current threshold of all the sensors: \n1) Temperature: ") + tlimit + "°C \n2) Humidity: " + hlimit + "% \n3) Air Quality: " + alimit + " ppm \n4) LPG: " + llimit + " ppm \n5) Carbon Monoxide: " + climit + " ppm \n6) Smoke: " + slimit + " ppm";
  temperature = String("Current Temperature: ") + Temperature + "°C";
  humidity = String("Current Humidity: ") + Humidity + "%";
  airquality = String("Current Air Quality: ") + PPM + " ppm";
  Lpg = String("Current LPG: ") + lpg + " ppm";
  carbon_monoxide = String("Current Carbon Monoxide: ") + co + " ppm";
  smk = String("Current Smoke: ") + smoke + " ppm";
  link = String("Click on the link to download: https://cloud.boltiot.com/downloadDataFromTable/Alert/<Bolt Wifi Module ID>?from=") + fdate + "+" + afhr + "%3A" + afmn + "&to=" + tdate + "+" + athr + "%3A" + atmn + "&duration=month";
  webserver = String("You can view real-time values and change the threshold values temporarily using the link (Connect to same access point of ESP8266): http://") + wifi ;
  fdate = details.substring(0,10);
  fhr = details.substring(11,13);
  fmn = details.substring(14,16);
  tdate = details.substring(17,27);
  thr = details.substring(28,30);
  tmn = details.substring(31,33);
  ftime = (60*fhr.toInt()+fmn.toInt())-330;
  ttime = (60*thr.toInt()+tmn.toInt())-330;
  afh = ftime/60;
  afm = ftime%60;
  ath = ttime/60;
  atm = ttime%60;
  if (afh<10) {
    afhr = "0" + String(afh);
  }
  else {
    afhr = String(afh);
  }
  if (ath<10) {
    athr = "0" + String(ath);
  }
  else {
    athr = String(ath);
  }
  if (afm<10) {
    afmn = "0" + String(afm);
  }
  else {
    afmn = String(afm);
  }
  if (atm<10) {
    atmn = "0" + String(atm);
  }
  else {
    atmn = String(atm);
  }
  if (Temperature <= tlimit.toFloat()) {
    T = 0;
  }
  else {
    T = 64;
    tmes = "🌡<b>Temperature</b>🌡 has crossed the threshold value of " + tlimit + "°C.\n";
}
  if (Humidity <= hlimit.toFloat()) {
    H = 0;
  }
  else {
    H = 32;
    hmes = "☁<b>Humidity</b>☁ has crossed the threshold value of " + hlimit + "%.\n";
}
  if (PPM <= alimit.toFloat()) {
    A = 0;
  }
  else {
    A = 16;
    ames = "😷<b>Air Quality</b>😷 has crossed the threshold value of " + alimit + " ppm.\n";
}
  if (lpg <= llimit.toFloat()) {
    L = 0;
  }
  else {
    L = 8;
    lmes = "⛽<b>Lpg</b>⛽ has crossed the threshold value of " + llimit + " ppm.\n";
}
  if (co <= climit.toFloat()) {
    C = 0;
  }
  else {
    C = 4;
    cmes = "🚬<b>Carbon Monoxide</b>🚬 has crossed the threshold value of " + climit + " ppm.\n";
}
  if (smoke <= slimit.toFloat()) {
    S = 0;
  }
  else {
    S = 2;
    smes = "💨<b>Smoke</b>💨 has crossed the threshold value of " + slimit + " ppm.\n";
}
  ch = T + H + A + L + C + S + FL;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, ch);
  digitalWrite(loadPin, HIGH);
  if (ch == 0) {
    noTone(Buzzer);  //No Sound from buzzer
  }
  else {
    tone(Buzzer, (ch * 80) + 250);  //change buzzer sound according to the Alert Number
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending Alert No." + String(ch));
    lcd.setCursor(0, 1);
    lcd.print("to Telegram Chat ID: ");
    lcd.setCursor(0, 2);
    lcd.print(CHAT_ID);
    // Sending Telegram Message
    bot.sendMessage("⚠<b><u>Alert No." + String(ch) + "</u></b>⚠\n" + tmes + hmes + ames + lmes + cmes + smes + fmes + readings + "\nImmediate action is required.");
    delay(2000);
    lcd.clear();
  }
  delay(1400);
}