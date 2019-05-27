/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-5-16
 * Description：common configurations for the SMHC Scheduling
 */

#ifndef CHAMELEON_CHAMELEON_SMHC_HPP
#define CHAMELEON_CHAMELEON_SMHC_HPP

#include <iostream>
#include <string>
using std::string;
namespace chameleon {
    namespace SMHC {

        //CPU Model
        const int a1 = 1;      //  Core i3
        const int a2 = 2;      //  Core i5
        const int a3 = 3;      //  Core i7
        const int a4 = 4;      //  Xeon E5

        //CPU Clock Speed(unit : GHz)
        const int b1 = 1;      //  0<=b<2
        const int b2 = 2;      //  2<=b<3
        const int b3 = 3;      //  b>=3

        //CPU L3 Cache(unit : MB)
        const int c1 = 1;      //  0<c<=10
        const int c2 = 2;      //  10<c<=20
        const int c3 = 3;      //  20<c<=30
        const int c4 = 4;      //  30<c

        //Memory Speed(unit : MHz)
        const int d1 = 1;      //  d<=2000
        const int d2 = 2;      //  d>=2000

        //Disk Speed(unit : MB/s)
        const int e1 = 1;      //  e<100
        const int e2 = 2;      //  e>=200
    }
}

#endif CHAMELEON_CHAMELEON_SMHC_HPP
