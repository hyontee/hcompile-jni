//
// Created by admin on 25.09.2023.
//

#ifndef CRMPONLINE_CBUYAUTO_H
#define CRMPONLINE_CBUYAUTO_H

#include "../main.h"


class CBuyAuto {
public:
    static void show();
    static void hide();

    static void addCarToRecycler(uint16_t vehicleModelId, int price, int maxSpeed, int maxFuel, float timeTo100, int availabilityInStock, char* name);

    static bool isShow() {
        return bShow;
    }

    static bool bShow;
};


#endif //CRMPONLINE_CBUYAUTO_H
