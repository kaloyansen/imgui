#ifndef K3KEY_H
#define K3KEY_H
#include <stdio.h>
#include <vector>

/**
 * @brief a class with methods to control windows
 *
 * a reasonable way to hide/show/flip gui windows
 */
class K3Key {
private:
     int bize;
     std::vector<bool> bector;
     bool out_of_range(int) const;
     void info(float, const char*) const;

public:
     K3Key(int size) : bector(size, false) { this->bize = size; } /*!< public constructor */
     bool* is(int); /*!< get a reference to a key */
     bool status(int) const; /*!< get the key value */

     void hide(void); /*!< desactivate all keys */
     void show(int, bool); /*!< activate a key */
     void flip(int); /*!< flip the key */
};

#endif
