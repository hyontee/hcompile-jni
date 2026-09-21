package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import com.google.gson.annotations.SerializedName;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: NetworkFamilySystem.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0002\b\f\n\u0002\u0010\u000b\n\u0002\b\u0004\b\u0087\b\u0018\u00002\u00020\u0001B\u001d\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0005¢\u0006\u0002\u0010\u0007J\t\u0010\r\u001a\u00020\u0003HÆ\u0003J\t\u0010\u000e\u001a\u00020\u0005HÆ\u0003J\t\u0010\u000f\u001a\u00020\u0005HÆ\u0003J'\u0010\u0010\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u0005HÆ\u0001J\u0013\u0010\u0011\u001a\u00020\u00122\b\u0010\u0013\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u0014\u001a\u00020\u0003HÖ\u0001J\t\u0010\u0015\u001a\u00020\u0005HÖ\u0001R\u0016\u0010\u0006\u001a\u00020\u00058\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\b\u0010\tR\u0016\u0010\u0002\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\n\u0010\u000bR\u0016\u0010\u0004\u001a\u00020\u00058\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\f\u0010\t¨\u0006\u0016"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemColorsObj;", "", "id", "", "title", "", "color", "(ILjava/lang/String;Ljava/lang/String;)V", "getColor", "()Ljava/lang/String;", "getId", "()I", "getTitle", "component1", "component2", "component3", "copy", "equals", "", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilySystemColorsObj {
    public static final int $stable = 0;

    @SerializedName("color")
    @NotNull
    public final String color;

    @SerializedName("id")
    public final int id;

    @SerializedName("title")
    @NotNull
    public final String title;

    public static /* synthetic */ FamilySystemColorsObj copy$default(FamilySystemColorsObj familySystemColorsObj, int i, String str, String str2, int i2, Object obj) {
        if ((i2 & 1) != 0) {
            i = familySystemColorsObj.id;
        }
        if ((i2 & 2) != 0) {
            str = familySystemColorsObj.title;
        }
        if ((i2 & 4) != 0) {
            str2 = familySystemColorsObj.color;
        }
        return familySystemColorsObj.copy(i, str, str2);
    }

    /* renamed from: component1, reason: from getter */
    public final int getId() {
        return this.id;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getTitle() {
        return this.title;
    }

    @NotNull
    /* renamed from: component3, reason: from getter */
    public final String getColor() {
        return this.color;
    }

    @NotNull
    public final FamilySystemColorsObj copy(int id, @NotNull String title, @NotNull String color) {
        Intrinsics.checkNotNullParameter(title, "title");
        Intrinsics.checkNotNullParameter(color, "color");
        return new FamilySystemColorsObj(id, title, color);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilySystemColorsObj)) {
            return false;
        }
        FamilySystemColorsObj familySystemColorsObj = (FamilySystemColorsObj) other;
        return this.id == familySystemColorsObj.id && Intrinsics.areEqual(this.title, familySystemColorsObj.title) && Intrinsics.areEqual(this.color, familySystemColorsObj.color);
    }

    public int hashCode() {
        return (((Integer.hashCode(this.id) * 31) + this.title.hashCode()) * 31) + this.color.hashCode();
    }

    @NotNull
    public String toString() {
        return "FamilySystemColorsObj(id=" + this.id + ", title=" + this.title + ", color=" + this.color + ")";
    }

    public FamilySystemColorsObj(int i, @NotNull String title, @NotNull String color) {
        Intrinsics.checkNotNullParameter(title, "title");
        Intrinsics.checkNotNullParameter(color, "color");
        this.id = i;
        this.title = title;
        this.color = color;
    }

    public final int getId() {
        return this.id;
    }

    @NotNull
    public final String getTitle() {
        return this.title;
    }

    @NotNull
    public final String getColor() {
        return this.color;
    }
}
