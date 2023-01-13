
      // send LED_intensity and LED_select to clients -> as optimization step one could send it just to the new client "num", but for simplicity I left that out here
      // sendJson("LED_intensity", String(LED_intensity));
      // sendJson("LED_selected", String(LED_selected));
      break;
    // case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
    //   // try to decipher the JSON string received
    //   StaticJsonDocument<200> doc;                    // create JSON container 
    //   DeserializationError error = deserializeJson(doc, payload);
    //   if (error) {
    //     Serial.print(F("deserializeJson() failed: "));
    //     Serial.println(error.f_str());
    //     return;
    //   }
    //   else {
    //     // JSON string was received correctly, so information can be retrieved:
    //     const char* type = doc["type"];
    //     const int value = doc["value"];
    //     Serial.println("New Event:\nType: " + String(type) + "\nValue: " + String(value));
    //     return;
    //   };
        // if LED_intensity value is received -> update and write to LED
      //   if(String(type) == "LED_intensity") {
      //     LED_intensity = int(value);
      //     sendJson("LED_intensity", String(value));
      //     ledcWrite(led_channels[LED_selected], map(LED_intensity, 0, 100, 0, 255));
      //   }
      //   // else if LED_select is changed -> switch on LED and switch off the rest
      //   if(String(type) == "LED_selected") {
      //     LED_selected = int(value);
      //     sendJson("LED_selected", String(value));
      //     for (int i = 0; i < 3; i++) {
      //       if (i == LED_selected)
      //         ledcWrite(led_channels[i], map(LED_intensity, 0, 100, 0, 255));       // switch on LED
      //       else
      //         ledcWrite(led_channels[i], 0);                                        // switch off not-selected LEDs 
      //     }
      //   }
      // }
      // Serial.println("");
      // break;