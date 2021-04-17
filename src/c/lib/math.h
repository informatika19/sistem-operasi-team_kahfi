/**
 * math.h
 * Alvin W., Josep M., Rehagana K.C.S.
 * 6 Maret 2021
 *
 * Header yang berisi fungsi-fungsi matematika
 */

#ifndef _MATH_CABECABE_H_
#define _MATH_CABECABE_H_

/**
 * Fungsi untuk menghitung modulo aritmatika dari a dan b.
 * a % b
 * @param a
 * @param b
 * @returns a mod b
 */
int mod(int a, int b);

/**
 * Fungsi untuk menghitung pembagian bilangan bulat
 * a // b kalau di Python
 * @param a
 * @param b
 * @returns div_result dengan { result = a//b,  remainder = a%b }
 */
int div(int a, int b);

#endif
