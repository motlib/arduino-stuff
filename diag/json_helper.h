/**
 * @file
 *
 * This file contains helper macros to generate JSON output with the Serial
 * class.
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H


#ifdef ARDUINO
#define JSON_STRWRAP(S)                         \
    F()

#define JSON_STR(S)                             \
    Serial.print(JSON_STRWRAP(S))

#define JSON_FLOAT(F)                           \
    Serial.print(F)

#define JSON_INT(I) \
    Serial.print(I)

#else

#define JSON_STRWRAP(S)                         \
    ()

#define JSON_STR(S)                             \
    printf("%s", (S))

#define JSON_FLOAT(F)                           \
    printf("%f", (F))

#define JSON_INT(I) \
    printf("%i", (I))

#endif /* ARDUINO */


#define JSON_KEY(NAME)                          \
    JSON_STR("\"");                             \
    JSON_STR(NAME);                             \
    JSON_STR("\":")

/**
 * Start output of a dictionary.
 */
#define JSON_DICT_START                         \
    JSON_STR("{")


/**
 * Add a float value to a dictionary.
 */
#define JSON_DICT_FLOAT(NAME,VAL)               \
    JSON_KEY(NAME);                             \
    JSON_FLOAT(VAL)

/**
 * Add a float value to a dictionary.
 */
#define JSON_DICT_INT(NAME, VAL)               \
    JSON_KEY(NAME);                           \
    JSON_INT(VAL)


/**
 * Add a string to a dictionary.
 */
#define JSON_DICT_STR(NAME,VAL)                 \
    JSON_KEY(NAME);                             \
    JSON_STR("\"");                             \
    JSON_STR(VAL);                              \
    JSON_STR("\"")


/**
 * End a dictionary.
 */
#define JSON_DICT_END \
    JSON_STR("}\n")


/**
 * Add a separator (comma) between dictionary items.
 */
#define JSON_SEP \
    JSON_STR(",")

#endif /* JSON_HELPER_H */

