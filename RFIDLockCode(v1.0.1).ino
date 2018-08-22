//Gives access to C Libraries (groups of pre-made functions). 
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#define SS_PIN 10 //Defines the corresponding arduino pin for SDA pin on MFRC (SDA or SS is 'slave select': select MFRC when the CPU wants to communicate.  
#define RST_PIN 9 //Defines the corresponding arduino pin for MFRC522 RST pin (RST is the reset pin)
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
Servo myServo; //Define servo name
String masterKeyID = "B0 C0 83 A6"; //Global variable that holds the Alphanumeric String of the MasterKeyID.
String userTagIDs[] = {"19 EF B4 63", "15 EC B6 81"}; //Global variable that holds an array of possible Alphanum
int correctEntries = 0; //Global vairable that logs the amount of correct Entries
int incorrectEntries = 0; //Global Variable that logs the amount of incorrect entries.
const int servoOpenPosition = 90; //Lets me change the angle the Servo rotates to if needed. I used this as a constant as its value does not change anywhere in the prgrogram. 
const int servoLockPosition = servoOpenPosition - servoOpenPosition; // i.e. = 0 (angle) rotates back to. I used this as a constant as its value does not change anywhere in the program. 
const int delayTime = 5; //a constant integer set to 5. This is to let me set a predetermined, unchanged amount of seconds delay.


//An inbuilt setup function provided by standard ardiuno libraries. 
void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate SPI bus - means I intialisse that I can now send data. 
  mfrc522.PCD_Init();   // Initiate the MFRC522 RFID reader. 
  myServo.attach(3); //Defines the corresponding arduino pin for Servo Control
  openLock(); //Opens the lock at initialisation. 
  Serial.println("Put your card to the reader..."); //FOR TESTING: Prints on screen to place card on reader. 
  Serial.println(); //FOR TESTING: Prints empty line (for screen aesthetic).
}

//An inbuilt loop function provided by standard ardiuno libraries. This runs constantly. 
void loop()
{
   // Look for new cards - if VALID card is present, continue. IF NO VALID CARD IS PRESENT, RETURN. 
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards - If ONE card with ONE valid serial ID is present, continue. 
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  ReadTag(); //Call the ReadTag Function. 
}

//ReadTag function: reads the tag and converts it to a readable Alphanumeric
void ReadTag() {
  //Show UID on serial monitor
  Serial.print("UID tag :"); //Display a line on the serial moniter that reads "UID tag :"
  String tagId = ""; //Defines local variable that will hold the Tag Id. 
  byte letter; //Defines a local variable that will hold the the 0 or 1 values for each corresponding letter in the end string displayed. 
  
  //The following for-loop converts the RFID tag UID into a readable string 
  for (byte i = 0; i < mfrc522.uid.size; i++)//Repeat the following code for each byte (8 bits of data) until the full size of the UID is finished. 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); //This checks if the RFID tag UID has 0x10 formatting i.e. this checks one type of RFID tag, and prints it. 
    Serial.print(mfrc522.uid.uidByte[i], HEX);//This checks if the RFID tag UID has HEX formatting i.e. this checks a different type of RFID tag, and prints it
    tagId.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ")); //Appends the UID to a the TagUI string if the tag has 0x10 formatting (returns true). 
    tagId.concat(String(mfrc522.uid.uidByte[i], HEX));//Appends the UID to a the TagUI string if the tag has HEX formatting (returns true).
  }
  Serial.println(); 
  Serial.print("Message : ");
  tagId.toUpperCase(); //converts the tagId to upper case

  //FOR BOUNDARY TEST - Uncomment following lines:
  /*tagId = "B0 C0 83 6";//FOR TESTING: TagId of incorrect length because, for example, it missed a character.
  Serial.print("Tag Test means the tag was changed to: " + tagId);
  Serial.println();*/

  //FOR EXCEPTIONAL TEST ONE - Uncomment following lines: 
  /*tagId = "ABCDEFG";//FOR TESTING: TagId of incorrect length because, for example, it uses non-alphanumerical data.
  Serial.print("Tag Test means the tag was changed to: " + tagId);
  Serial.println();*/

  //FOR EXCEPTIONAL TEST TWO - Uncomment following lines: 
  /*tagId = "1";//FOR TESTING: TagId of incorrect length because, for example, it reads the int 1 inside it's string value.
  Serial.print("Tag Test means the tag was changed to: " + tagId);
  Serial.println();*/
  
  TagTest(tagId); //Calls the function TagTest and passes tagId as an argument.
}

//TagTest function: Checks if Tag matches any it has. 
void TagTest(String ID) {
  bool found = false;
  int n = sizeof(userTagIDs)/sizeof(String); //A local variable that stores the value of the UserTagID's array length.
  if (ID.substring(1) ==  masterKeyID)
  { 
    correctEntries += 1; //increments correctEntries
    Serial.println("Authorised access"); //prints authorised access on the serial monitor 
    Serial.print(correctEntries); //FOR TESTING: Prints the amount of correct Entries
    Serial.println();
    openLock(); //This calls the function. 'openLock'
    delay(delayTime * 1000); //Wait one second (value is in milliseconds) - will change to 10-15 seconds once 
    closeLock(); //This calls the function 'closeLock'
    found = true;
  }
  else
  {  
    for (int i = 0; i < n; i++)
    {
      if(ID.substring(1) == userTagIDs[i])
      {
        correctEntries += 1; //increments correctEntries
        Serial.println("Authorised access"); //prints authorised access on the serial monitor 
        Serial.print(correctEntries); //FOR TESTING: Prints the amount of correct Entries
        Serial.println();
        openLock(); //This calls the function. 'openLock'
        delay(delayTime * 1000); //Wait one second (value is in milliseconds) - will change to 10-15 seconds once 
        closeLock(); //This calls the function 'closeLock'
        found = true;
      }
    }
  }
  if (!found)   
  {
  incorrectEntries += 1; //increments incorrectEntries
  Serial.println(" Access denied"); //prints access denied on the serial monitor
  Serial.print(incorrectEntries); //FOR TESTING: Prints the amount of Incorrect Entries
  delay(1 * 1000);
  }
}

//Functions that open or close the lock i.e. move the servo
void openLock() {
  myServo.write(servoOpenPosition);
}
void closeLock() {
  myServo.write(servoLockPosition);
}

  
