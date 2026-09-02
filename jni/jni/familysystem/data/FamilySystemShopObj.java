package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import com.google.firebase.analytics.FirebaseAnalytics;
import com.google.gson.annotations.SerializedName;
import kotlin.Metadata;
import kotlin.jvm.internal.DefaultConstructorMarker;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: NetworkFamilySystem.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0006\n\u0002\u0010\u000b\n\u0002\b\u001b\b\u0087\b\u0018\u00002\u00020\u0001BG\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0003\u0012\u0006\u0010\u0007\u001a\u00020\u0003\u0012\u0006\u0010\b\u001a\u00020\u0003\u0012\u0006\u0010\t\u001a\u00020\u0003\u0012\u0006\u0010\n\u001a\u00020\u0003\u0012\b\b\u0002\u0010\u000b\u001a\u00020\f¢\u0006\u0002\u0010\rJ\t\u0010\u001a\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001b\u001a\u00020\u0005HÆ\u0003J\t\u0010\u001c\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001d\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001e\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001f\u001a\u00020\u0003HÆ\u0003J\t\u0010 \u001a\u00020\u0003HÆ\u0003J\t\u0010!\u001a\u00020\fHÆ\u0003JY\u0010\"\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u00032\b\b\u0002\u0010\u0007\u001a\u00020\u00032\b\b\u0002\u0010\b\u001a\u00020\u00032\b\b\u0002\u0010\t\u001a\u00020\u00032\b\b\u0002\u0010\n\u001a\u00020\u00032\b\b\u0002\u0010\u000b\u001a\u00020\fHÆ\u0001J\u0013\u0010#\u001a\u00020\f2\b\u0010$\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010%\u001a\u00020\u0003HÖ\u0001J\t\u0010&\u001a\u00020\u0005HÖ\u0001R\u0016\u0010\u0002\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u000e\u0010\u000fR\u001a\u0010\u000b\u001a\u00020\fX\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u000b\u0010\u0010\"\u0004\b\u0011\u0010\u0012R\u0016\u0010\u0006\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\u000fR\u0016\u0010\t\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0014\u0010\u000fR\u0016\u0010\n\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0015\u0010\u000fR\u0016\u0010\u0004\u001a\u00020\u00058\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0016\u0010\u0017R\u0016\u0010\u0007\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0018\u0010\u000fR\u0016\u0010\b\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0019\u0010\u000f¨\u0006'"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;", "", "id", "", "title", "", "objectId", "type", "typeId", FirebaseAnalytics.Param.PRICE, "serverId", "isClicked", "", "(ILjava/lang/String;IIIIIZ)V", "getId", "()I", "()Z", "setClicked", "(Z)V", "getObjectId", "getPrice", "getServerId", "getTitle", "()Ljava/lang/String;", "getType", "getTypeId", "component1", "component2", "component3", "component4", "component5", "component6", "component7", "component8", "copy", "equals", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilySystemShopObj {
    public static final int $stable = 8;

    @SerializedName("id")
    public final int id;
    public boolean isClicked;

    @SerializedName("object_id")
    public final int objectId;

    @SerializedName(FirebaseAnalytics.Param.PRICE)
    public final int price;

    @SerializedName("server_id")
    public final int serverId;

    @SerializedName("title")
    @NotNull
    public final String title;

    @SerializedName("type")
    public final int type;

    @SerializedName("type_id")
    public final int typeId;

    /* renamed from: component1, reason: from getter */
    public final int getId() {
        return this.id;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getTitle() {
        return this.title;
    }

    /* renamed from: component3, reason: from getter */
    public final int getObjectId() {
        return this.objectId;
    }

    /* renamed from: component4, reason: from getter */
    public final int getType() {
        return this.type;
    }

    /* renamed from: component5, reason: from getter */
    public final int getTypeId() {
        return this.typeId;
    }

    /* renamed from: component6, reason: from getter */
    public final int getPrice() {
        return this.price;
    }

    /* renamed from: component7, reason: from getter */
    public final int getServerId() {
        return this.serverId;
    }

    /* renamed from: component8, reason: from getter */
    public final boolean getIsClicked() {
        return this.isClicked;
    }

    @NotNull
    public final FamilySystemShopObj copy(int id, @NotNull String title, int objectId, int type, int typeId, int price, int serverId, boolean isClicked) {
        Intrinsics.checkNotNullParameter(title, "title");
        return new FamilySystemShopObj(id, title, objectId, type, typeId, price, serverId, isClicked);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilySystemShopObj)) {
            return false;
        }
        FamilySystemShopObj familySystemShopObj = (FamilySystemShopObj) other;
        return this.id == familySystemShopObj.id && Intrinsics.areEqual(this.title, familySystemShopObj.title) && this.objectId == familySystemShopObj.objectId && this.type == familySystemShopObj.type && this.typeId == familySystemShopObj.typeId && this.price == familySystemShopObj.price && this.serverId == familySystemShopObj.serverId && this.isClicked == familySystemShopObj.isClicked;
    }

    public int hashCode() {
        return (((((((((((((Integer.hashCode(this.id) * 31) + this.title.hashCode()) * 31) + Integer.hashCode(this.objectId)) * 31) + Integer.hashCode(this.type)) * 31) + Integer.hashCode(this.typeId)) * 31) + Integer.hashCode(this.price)) * 31) + Integer.hashCode(this.serverId)) * 31) + Boolean.hashCode(this.isClicked);
    }

    @NotNull
    public String toString() {
        return "FamilySystemShopObj(id=" + this.id + ", title=" + this.title + ", objectId=" + this.objectId + ", type=" + this.type + ", typeId=" + this.typeId + ", price=" + this.price + ", serverId=" + this.serverId + ", isClicked=" + this.isClicked + ")";
    }

    public FamilySystemShopObj(int i, @NotNull String title, int i2, int i3, int i4, int i5, int i6, boolean z) {
        Intrinsics.checkNotNullParameter(title, "title");
        this.id = i;
        this.title = title;
        this.objectId = i2;
        this.type = i3;
        this.typeId = i4;
        this.price = i5;
        this.serverId = i6;
        this.isClicked = z;
    }

    public /* synthetic */ FamilySystemShopObj(int i, String str, int i2, int i3, int i4, int i5, int i6, boolean z, int i7, DefaultConstructorMarker defaultConstructorMarker) {
        this(i, str, i2, i3, i4, i5, i6, (i7 & 128) != 0 ? false : z);
    }

    public final int getId() {
        return this.id;
    }

    @NotNull
    public final String getTitle() {
        return this.title;
    }

    public final int getObjectId() {
        return this.objectId;
    }

    public final int getType() {
        return this.type;
    }

    public final int getTypeId() {
        return this.typeId;
    }

    public final int getPrice() {
        return this.price;
    }

    public final int getServerId() {
        return this.serverId;
    }

    public final boolean isClicked() {
        return this.isClicked;
    }

    public final void setClicked(boolean z) {
        this.isClicked = z;
    }
}
