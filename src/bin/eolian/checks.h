#ifndef CHECKS_H
#define CHECKS_H

#include "main.h"

/**
 * @brief Apply checks on the given filename
 *
 * @param[in] src the parsed eolian unit
 * @param[in] filename the filename to check
 *
 */
void apply_checks(const Eolian_Unit *src, const char *filename);

#endif
