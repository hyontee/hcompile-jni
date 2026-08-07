package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import com.google.firebase.analytics.FirebaseAnalytics;
import com.google.gson.annotations.SerializedName;
import kotlin.Metadata;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: NetworkFamilySystem.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0002\b!\n\u0002\u0010\u000b\n\u0002\b\u0003\n\u0002\u0010\u000e\n\u0000\b\u0087\b\u0018\u00002\u00020\u0001BU\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0003\u0012\u0006\u0010\u0005\u001a\u00020\u0003\u0012\u0006\u0010\u0006\u001a\u00020\u0003\u0012\u0006\u0010\u0007\u001a\u00020\u0003\u0012\u0006\u0010\b\u001a\u00020\u0003\u0012\u0006\u0010\t\u001a\u00020\u0003\u0012\u0006\u0010\n\u001a\u00020\u0003\u0012\u0006\u0010\u000b\u001a\u00020\u0003\u0012\u0006\u0010\f\u001a\u00020\u0003¢\u0006\u0002\u0010\rJ\t\u0010\u0019\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001a\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001b\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001c\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001d\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001e\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001f\u001a\u00020\u0003HÆ\u0003J\t\u0010 \u001a\u00020\u0003HÆ\u0003J\t\u0010!\u001a\u00020\u0003HÆ\u0003J\t\u0010\"\u001a\u00020\u0003HÆ\u0003Jm\u0010#\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00032\b\b\u0002\u0010\u0005\u001a\u00020\u00032\b\b\u0002\u0010\u0006\u001a\u00020\u00032\b\b\u0002\u0010\u0007\u001a\u00020\u00032\b\b\u0002\u0010\b\u001a\u00020\u00032\b\b\u0002\u0010\t\u001a\u00020\u00032\b\b\u0002\u0010\n\u001a\u00020\u00032\b\b\u0002\u0010\u000b\u001a\u00020\u00032\b\b\u0002\u0010\f\u001a\u00020\u0003HÆ\u0001J\u0013\u0010$\u001a\u00020%2\b\u0010&\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010'\u001a\u00020\u0003HÖ\u0001J\t\u0010(\u001a\u00020)HÖ\u0001R\u0016\u0010\u000b\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u000e\u0010\u000fR\u0016\u0010\t\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0010\u0010\u000fR\u0016\u0010\b\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0011\u0010\u000fR\u0016\u0010\u0002\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0012\u0010\u000fR\u0016\u0010\u0004\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\u000fR\u0016\u0010\u0007\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0014\u0010\u000fR\u0016\u0010\u0006\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0015\u0010\u000fR\u0016\u0010\u0005\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0016\u0010\u000fR\u0016\u0010\n\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0017\u0010\u000fR\u0016\u0010\f\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0018\u0010\u000f¨\u0006*"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemUpgradeObj;", "", "id", "", "level", "money", "materials", "masks", "gasoline", "armour", "people", "ammo", FirebaseAnalytics.Param.PRICE, "(IIIIIIIIII)V", "getAmmo", "()I", "getArmour", "getGasoline", "getId", "getLevel", "getMasks", "getMaterials", "getMoney", "getPeople", "getPrice", "component1", "component10", "component2", "component3", "component4", "component5", "component6", "component7", "component8", "component9", "copy", "equals", "", "other", "hashCode", "toString", "", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilySystemUpgradeObj {
    public static final int $stable = 0;

    @SerializedName("ammo")
    public final int ammo;

    @SerializedName("armour")
    public final int armour;

    @SerializedName("gasoline")
    public final int gasoline;

    @SerializedName("id")
    public final int id;

    @SerializedName("level")
    public final int level;

    @SerializedName("masks")
    public final int masks;

    @SerializedName("materials")
    public final int materials;

    @SerializedName("money")
    public final int money;

    @SerializedName("people")
    public final int people;

    @SerializedName(FirebaseAnalytics.Param.PRICE)
    public final int price;

    /* renamed from: component1, reason: from getter */
    public final int getId() {
        return this.id;
    }

    /* renamed from: component10, reason: from getter */
    public final int getPrice() {
        return this.price;
    }

    /* renamed from: component2, reason: from getter */
    public final int getLevel() {
        return this.level;
    }

    /* renamed from: component3, reason: from getter */
    public final int getMoney() {
        return this.money;
    }

    /* renamed from: component4, reason: from getter */
    public final int getMaterials() {
        return this.materials;
    }

    /* renamed from: component5, reason: from getter */
    public final int getMasks() {
        return this.masks;
    }

    /* renamed from: component6, reason: from getter */
    public final int getGasoline() {
        return this.gasoline;
    }

    /* renamed from: component7, reason: from getter */
    public final int getArmour() {
        return this.armour;
    }

    /* renamed from: component8, reason: from getter */
    public final int getPeople() {
        return this.people;
    }

    /* renamed from: component9, reason: from getter */
    public final int getAmmo() {
        return this.ammo;
    }

    @NotNull
    public final FamilySystemUpgradeObj copy(int id, int level, int money, int materials, int masks, int gasoline, int armour, int people, int ammo, int price) {
        return new FamilySystemUpgradeObj(id, level, money, materials, masks, gasoline, armour, people, ammo, price);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilySystemUpgradeObj)) {
            return false;
        }
        FamilySystemUpgradeObj familySystemUpgradeObj = (FamilySystemUpgradeObj) other;
        return this.id == familySystemUpgradeObj.id && this.level == familySystemUpgradeObj.level && this.money == familySystemUpgradeObj.money && this.materials == familySystemUpgradeObj.materials && this.masks == familySystemUpgradeObj.masks && this.gasoline == familySystemUpgradeObj.gasoline && this.armour == familySystemUpgradeObj.armour && this.people == familySystemUpgradeObj.people && this.ammo == familySystemUpgradeObj.ammo && this.price == familySystemUpgradeObj.price;
    }

    public int hashCode() {
        return (((((((((((((((((Integer.hashCode(this.id) * 31) + Integer.hashCode(this.level)) * 31) + Integer.hashCode(this.money)) * 31) + Integer.hashCode(this.materials)) * 31) + Integer.hashCode(this.masks)) * 31) + Integer.hashCode(this.gasoline)) * 31) + Integer.hashCode(this.armour)) * 31) + Integer.hashCode(this.people)) * 31) + Integer.hashCode(this.ammo)) * 31) + Integer.hashCode(this.price);
    }

    @NotNull
    public String toString() {
        return "FamilySystemUpgradeObj(id=" + this.id + ", level=" + this.level + ", money=" + this.money + ", materials=" + this.materials + ", masks=" + this.masks + ", gasoline=" + this.gasoline + ", armour=" + this.armour + ", people=" + this.people + ", ammo=" + this.ammo + ", price=" + this.price + ")";
    }

    public FamilySystemUpgradeObj(int i, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10) {
        this.id = i;
        this.level = i2;
        this.money = i3;
        this.materials = i4;
        this.masks = i5;
        this.gasoline = i6;
        this.armour = i7;
        this.people = i8;
        this.ammo = i9;
        this.price = i10;
    }

    public final int getId() {
        return this.id;
    }

    public final int getLevel() {
        return this.level;
    }

    public final int getMoney() {
        return this.money;
    }

    public final int getMaterials() {
        return this.materials;
    }

    public final int getMasks() {
        return this.masks;
    }

    public final int getGasoline() {
        return this.gasoline;
    }

    public final int getArmour() {
        return this.armour;
    }

    public final int getPeople() {
        return this.people;
    }

    public final int getAmmo() {
        return this.ammo;
    }

    public final int getPrice() {
        return this.price;
    }
}
