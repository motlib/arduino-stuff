#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#define JSON_DICT_START                         \
    Serial.print(F("{"))

#define JSON_DICT_FLOAT(NAME,VAL)               \
    Serial.print(F("\""));                      \
    Serial.print(NAME);                         \
    Serial.print(F("\":"));                     \
    Serial.print(VAL)

#define JSON_DICT_STR(NAME,VAL)                 \
    Serial.print(F("\""));                      \
    Serial.print(NAME);                         \
    Serial.print(F("\""));                      \
    Serial.print(F(":"));                       \
    Serial.print(F("\""));                      \
    Serial.print(VAL);                          \
    Serial.print(F("\""))

#define JSON_DICT_END \
    Serial.print(F("}\n"))

#define JSON_SEP \
    Serial.print(F(","))

#endif /* JSON_HELPER_H */

