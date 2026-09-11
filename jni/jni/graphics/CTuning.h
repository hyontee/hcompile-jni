//
// Created by admin on 26.10.2023.
//

#pragma once


class CTuning {
public:
    static void show();
    static void hide();

    static bool isShow() {
        return bShow;
    }

    static bool bShow;
};
