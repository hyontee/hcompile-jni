package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
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
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0005\n\u0002\u0010\u000b\n\u0002\b\u0019\b\u0087\b\u0018\u00002\u00020\u0001B?\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0005\u0012\u0006\u0010\u0007\u001a\u00020\u0003\u0012\u0006\u0010\b\u001a\u00020\u0003\u0012\u0006\u0010\t\u001a\u00020\u0003\u0012\b\b\u0002\u0010\n\u001a\u00020\u000b¢\u0006\u0002\u0010\fJ\t\u0010\u0018\u001a\u00020\u0003HÆ\u0003J\t\u0010\u0019\u001a\u00020\u0005HÆ\u0003J\t\u0010\u001a\u001a\u00020\u0005HÆ\u0003J\t\u0010\u001b\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001c\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001d\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001e\u001a\u00020\u000bHÆ\u0003JO\u0010\u001f\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u00052\b\b\u0002\u0010\u0007\u001a\u00020\u00032\b\b\u0002\u0010\b\u001a\u00020\u00032\b\b\u0002\u0010\t\u001a\u00020\u00032\b\b\u0002\u0010\n\u001a\u00020\u000bHÆ\u0001J\u0013\u0010 \u001a\u00020\u000b2\b\u0010!\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\"\u001a\u00020\u0003HÖ\u0001J\t\u0010#\u001a\u00020\u0005HÖ\u0001R\u0016\u0010\u0006\u001a\u00020\u00058\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\r\u0010\u000eR\u0016\u0010\u0002\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u000f\u0010\u0010R\u001a\u0010\n\u001a\u00020\u000bX\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\n\u0010\u0011\"\u0004\b\u0012\u0010\u0013R\u0016\u0010\u0007\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0014\u0010\u0010R\u0016\u0010\b\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0015\u0010\u0010R\u0016\u0010\u0004\u001a\u00020\u00058\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0016\u0010\u000eR\u0016\u0010\t\u001a\u00020\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0017\u0010\u0010¨\u0006$"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemTasksObj;", "", "id", "", "title", "", "description", "moneyReward", "scoreReward", "tokenReward", "isClicked", "", "(ILjava/lang/String;Ljava/lang/String;IIIZ)V", "getDescription", "()Ljava/lang/String;", "getId", "()I", "()Z", "setClicked", "(Z)V", "getMoneyReward", "getScoreReward", "getTitle", "getTokenReward", "component1", "component2", "component3", "component4", "component5", "component6", "component7", "copy", "equals", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilySystemTasksObj {
    public static final int $stable = 8;

    @SerializedName("description")
    @NotNull
    public final String description;

    @SerializedName("id")
    public final int id;
    public boolean isClicked;

    @SerializedName("money_reward")
    public final int moneyReward;

    @SerializedName("score_reward")
    public final int scoreReward;

    @SerializedName("title")
    @NotNull
    public final String title;

    @SerializedName("token_reward")
    public final int tokenReward;

    public static /* synthetic */ FamilySystemTasksObj copy$default(FamilySystemTasksObj familySystemTasksObj, int i, String str, String str2, int i2, int i3, int i4, boolean z, int i5, Object obj) {
        if ((i5 & 1) != 0) {
            i = familySystemTasksObj.id;
        }
        if ((i5 & 2) != 0) {
            str = familySystemTasksObj.title;
        }
        String str3 = str;
        if ((i5 & 4) != 0) {
            str2 = familySystemTasksObj.description;
        }
        String str4 = str2;
        if ((i5 & 8) != 0) {
            i2 = familySystemTasksObj.moneyReward;
        }
        int i6 = i2;
        if ((i5 & 16) != 0) {
            i3 = familySystemTasksObj.scoreReward;
        }
        int i7 = i3;
        if ((i5 & 32) != 0) {
            i4 = familySystemTasksObj.tokenReward;
        }
        int i8 = i4;
        if ((i5 & 64) != 0) {
            z = familySystemTasksObj.isClicked;
        }
        return familySystemTasksObj.copy(i, str3, str4, i6, i7, i8, z);
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
    public final String getDescription() {
        return this.description;
    }

    /* renamed from: component4, reason: from getter */
    public final int getMoneyReward() {
        return this.moneyReward;
    }

    /* renamed from: component5, reason: from getter */
    public final int getScoreReward() {
        return this.scoreReward;
    }

    /* renamed from: component6, reason: from getter */
    public final int getTokenReward() {
        return this.tokenReward;
    }

    /* renamed from: component7, reason: from getter */
    public final boolean getIsClicked() {
        return this.isClicked;
    }

    @NotNull
    public final FamilySystemTasksObj copy(int id, @NotNull String title, @NotNull String description, int moneyReward, int scoreReward, int tokenReward, boolean isClicked) {
        Intrinsics.checkNotNullParameter(title, "title");
        Intrinsics.checkNotNullParameter(description, "description");
        return new FamilySystemTasksObj(id, title, description, moneyReward, scoreReward, tokenReward, isClicked);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilySystemTasksObj)) {
            return false;
        }
        FamilySystemTasksObj familySystemTasksObj = (FamilySystemTasksObj) other;
        return this.id == familySystemTasksObj.id && Intrinsics.areEqual(this.title, familySystemTasksObj.title) && Intrinsics.areEqual(this.description, familySystemTasksObj.description) && this.moneyReward == familySystemTasksObj.moneyReward && this.scoreReward == familySystemTasksObj.scoreReward && this.tokenReward == familySystemTasksObj.tokenReward && this.isClicked == familySystemTasksObj.isClicked;
    }

    public int hashCode() {
        return (((((((((((Integer.hashCode(this.id) * 31) + this.title.hashCode()) * 31) + this.description.hashCode()) * 31) + Integer.hashCode(this.moneyReward)) * 31) + Integer.hashCode(this.scoreReward)) * 31) + Integer.hashCode(this.tokenReward)) * 31) + Boolean.hashCode(this.isClicked);
    }

    @NotNull
    public String toString() {
        return "FamilySystemTasksObj(id=" + this.id + ", title=" + this.title + ", description=" + this.description + ", moneyReward=" + this.moneyReward + ", scoreReward=" + this.scoreReward + ", tokenReward=" + this.tokenReward + ", isClicked=" + this.isClicked + ")";
    }

    public FamilySystemTasksObj(int i, @NotNull String title, @NotNull String description, int i2, int i3, int i4, boolean z) {
        Intrinsics.checkNotNullParameter(title, "title");
        Intrinsics.checkNotNullParameter(description, "description");
        this.id = i;
        this.title = title;
        this.description = description;
        this.moneyReward = i2;
        this.scoreReward = i3;
        this.tokenReward = i4;
        this.isClicked = z;
    }

    public /* synthetic */ FamilySystemTasksObj(int i, String str, String str2, int i2, int i3, int i4, boolean z, int i5, DefaultConstructorMarker defaultConstructorMarker) {
        this(i, str, str2, i2, i3, i4, (i5 & 64) != 0 ? false : z);
    }

    public final int getId() {
        return this.id;
    }

    @NotNull
    public final String getTitle() {
        return this.title;
    }

    @NotNull
    public final String getDescription() {
        return this.description;
    }

    public final int getMoneyReward() {
        return this.moneyReward;
    }

    public final int getScoreReward() {
        return this.scoreReward;
    }

    public final int getTokenReward() {
        return this.tokenReward;
    }

    public final boolean isClicked() {
        return this.isClicked;
    }

    public final void setClicked(boolean z) {
        this.isClicked = z;
    }
}
