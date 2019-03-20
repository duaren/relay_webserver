/*
* Duaren Coding
* Serveur Web pour control de Relay 5V
* 20/03/2019
*/
#include <SPI.h> // Libairie communicatin SPI avec le Shield éthernet
#include <Ethernet.h> // Librairie communication Ethernet sur un réseau local
//On donne l'adresse MAC au shield Ethernet
//(ne doit pas être identique à une autre adresse MAC présente sur le réseau)
byte mac[] = {
0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED // Adresse MAC du shield Ethernet
};
//On affecte l'adresse IP du Shield Ethernet
IPAddress ip(192,168,1,20); // Adresse IP donné au Shield Ethernet
//On attribue la fonction serveur au Shield Ethernet sur le port 80
//Le Shield devient un serveur sur le port 80 (port HTTP)
EthernetServer server(80);
// Etat du relais et déclaration du port de sortie
String relayState = "OFF";
const int relay = 8;
// Déclaration des variables Client
char linebuf[80];
int charcount=0;void setup() {
// Préparation du module relais
pinMode(relay, OUTPUT);
digitalWrite(relay, HIGH);
// Initialisation de la communication série
Serial.begin(9600);
//Lancement de la connexion Ethernet avec l'adresse MAC et l'adresse IP
Ethernet.begin(mac, ip);
server.begin();
//On affiche l'adresse IP du serveur sur le moniteur série
Serial.print("Le serveur a pour adresse IP: ");
Serial.println(Ethernet.localIP());
}
// Affichage de la page on/off button pour le relais
void dashboardPage(EthernetClient &client) {
client.println("<!DOCTYPE HTML><html><head>");
client.println("<style> body {background-color:#f0f0f0;font-family: verdana;}" );
client.println("div {background-color:Skyblue;margin: auto;width: 60%;border-radius: 5px;padding:5px;}");
client.println("h1 {color: DodgerBlue;font-size: 25px;text-align: center;}");
client.println("h3{font-family: verdana;font-size: 20px;text-align: center;}");
client.println("h4{text-align: center;color:white;}");
client.println("a {font-size: 12px;}");
client.println("table { margin: auto;width:5%;align:center;border:solid 1px;border-radius: 3px;}");
client.println("</style>");
client.println("<meta name=\"viewport\" content=\"width=device-width, initialscale=1\"></head><body>");
client.println("<div><h1>Renauduino 2.0</h1>");
client.println("<h3> Serveur Web Arduino <a href=\"/\">Rafraichir</a></h3>");// Création du bouton pour controler le relais
client.println("<h4>Relay 5v - Etat: " + relayState + "</h4>");
// Si le relais est éteint, on montre le bouton pour mettre le output sur "on"
if(relayState == "OFF"){
client.println("<table><tr><td><a
href=\"/relayon\"><button>ON</button></a></tr></td></table>");
}
// Si le relais est allumer, on montre le bouton pour mettre le output sur "off"
else if(relayState == "ON"){
client.println("<table><tr><td><a href=\"/relayoff\"><button>OFF</button></a></tr></td></table>");
}
client.println("</div></body></html>");
}
void loop() {
// A l'écoute de nouveaux clients
EthernetClient client = server.available();
if (client) {
Serial.println("Nouveau client");
memset(linebuf,0,sizeof(linebuf));
charcount=0;
// une requette http fini avec une ligne vide 'blank line'
boolean currentLineIsBlank = true;
while (client.connected()) {
if (client.available()) {
char c = client.read();
// lecture caractère par caractère de la requête
linebuf[charcount]=c;
if (charcount<sizeof(linebuf)-1) charcount++;
// Si on arrive à la fin de la ligne et que la ligne qui suit est vide, la requète http est achevée,
// alors on peut envoyer la reponseif (c == '\n' && currentLineIsBlank) {
dashboardPage(client);
break;
}
if (c == '\n') {
if (strstr(linebuf,"GET /relayoff") > 0){
digitalWrite(relay, HIGH);
relayState = "OFF";
}
else if (strstr(linebuf,"GET /relayon") > 0){
digitalWrite(relay, LOW);
relayState = "ON";
}
// on commence une nouvelle ligne
currentLineIsBlank = true;
memset(linebuf,0,sizeof(linebuf));
charcount=0;
}
else if (c != '\r') {
// on a eu un caractère a sur la ligne courante
currentLineIsBlank = false;
}
}
}
// Donner au naviguateur le temps de recevoir les données
delay(1);
//Fermer la connexion
client.stop();
Serial.println("client déconnecté");
}
}
