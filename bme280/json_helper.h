/**
 * @file
 *
 * This file contains helper macros to generate JSON output with the Serial
 * class.
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H


/**
 * Start output of a dictionary.
 */
#define JSON_DICT_START                         \
    Serial.print(F("{"))


/**
 * Add a float value to a dictionary.
 */
#define JSON_DICT_FLOAT(NAME,VAL)               \
    Serial.print(F("\""));                      \
    Serial.print(NAME);                         \
    Serial.print(F("\":"));                     \
    Serial.print(VAL)


/**
 * Add a string to a dictionary.
 */
#define JSON_DICT_STR(NAME,VAL)                 \
    Serial.print(F("\""));                      \
    Serial.print(NAME);                         \
    Serial.print(F("\""));                      \
    Serial.print(F(":"));                       \
    Serial.print(F("\""));                      \
    Serial.print(VAL);                          \
    Serial.print(F("\""))


/**
 * End a dictionary.
 */
#define JSON_DICT_END \
    Serial.print(F("}\n"))


/**
 * Add a separator (comma) between dictionary items.
 */
#define JSON_SEP \
    Serial.print(F(","))

#endif /* JSON_HELPER_H */

