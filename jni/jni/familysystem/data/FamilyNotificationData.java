package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyNotificationData.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u0000\"\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\b\u000b\n\u0002\u0010\u000b\n\u0002\b\u0004\b\u0087\b\u0018\u00002\u00020\u0001B\u001d\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0003\u0012\u0006\u0010\u0005\u001a\u00020\u0006¢\u0006\u0002\u0010\u0007J\t\u0010\r\u001a\u00020\u0003HÆ\u0003J\t\u0010\u000e\u001a\u00020\u0003HÆ\u0003J\t\u0010\u000f\u001a\u00020\u0006HÆ\u0003J'\u0010\u0010\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00032\b\b\u0002\u0010\u0005\u001a\u00020\u0006HÆ\u0001J\u0013\u0010\u0011\u001a\u00020\u00122\b\u0010\u0013\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u0014\u001a\u00020\u0006HÖ\u0001J\t\u0010\u0015\u001a\u00020\u0003HÖ\u0001R\u0011\u0010\u0004\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\b\u0010\tR\u0011\u0010\u0005\u001a\u00020\u0006¢\u0006\b\n\u0000\u001a\u0004\b\n\u0010\u000bR\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\f\u0010\t¨\u0006\u0016"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "", "notificationsValue", "", "notificationsAuthor", "notificationsId", "", "(Ljava/lang/String;Ljava/lang/String;I)V", "getNotificationsAuthor", "()Ljava/lang/String;", "getNotificationsId", "()I", "getNotificationsValue", "component1", "component2", "component3", "copy", "equals", "", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyNotificationData {
    public static final int $stable = 0;

    @NotNull
    public final String notificationsAuthor;
    public final int notificationsId;

    @NotNull
    public final String notificationsValue;

    public static /* synthetic */ FamilyNotificationData copy$default(FamilyNotificationData familyNotificationData, String str, String str2, int i, int i2, Object obj) {
        if ((i2 & 1) != 0) {
            str = familyNotificationData.notificationsValue;
        }
        if ((i2 & 2) != 0) {
            str2 = familyNotificationData.notificationsAuthor;
        }
        if ((i2 & 4) != 0) {
            i = familyNotificationData.notificationsId;
        }
        return familyNotificationData.copy(str, str2, i);
    }

    @NotNull
    /* renamed from: component1, reason: from getter */
    public final String getNotificationsValue() {
        return this.notificationsValue;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getNotificationsAuthor() {
        return this.notificationsAuthor;
    }

    /* renamed from: component3, reason: from getter */
    public final int getNotificationsId() {
        return this.notificationsId;
    }

    @NotNull
    public final FamilyNotificationData copy(@NotNull String notificationsValue, @NotNull String notificationsAuthor, int notificationsId) {
        Intrinsics.checkNotNullParameter(notificationsValue, "notificationsValue");
        Intrinsics.checkNotNullParameter(notificationsAuthor, "notificationsAuthor");
        return new FamilyNotificationData(notificationsValue, notificationsAuthor, notificationsId);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyNotificationData)) {
            return false;
        }
        FamilyNotificationData familyNotificationData = (FamilyNotificationData) other;
        return Intrinsics.areEqual(this.notificationsValue, familyNotificationData.notificationsValue) && Intrinsics.areEqual(this.notificationsAuthor, familyNotificationData.notificationsAuthor) && this.notificationsId == familyNotificationData.notificationsId;
    }

    public int hashCode() {
        return (((this.notificationsValue.hashCode() * 31) + this.notificationsAuthor.hashCode()) * 31) + Integer.hashCode(this.notificationsId);
    }

    @NotNull
    public String toString() {
        return "FamilyNotificationData(notificationsValue=" + this.notificationsValue + ", notificationsAuthor=" + this.notificationsAuthor + ", notificationsId=" + this.notificationsId + ")";
    }

    public FamilyNotificationData(@NotNull String notificationsValue, @NotNull String notificationsAuthor, int i) {
        Intrinsics.checkNotNullParameter(notificationsValue, "notificationsValue");
        Intrinsics.checkNotNullParameter(notificationsAuthor, "notificationsAuthor");
        this.notificationsValue = notificationsValue;
        this.notificationsAuthor = notificationsAuthor;
        this.notificationsId = i;
    }

    @NotNull
    public final String getNotificationsValue() {
        return this.notificationsValue;
    }

    @NotNull
    public final String getNotificationsAuthor() {
        return this.notificationsAuthor;
    }

    public final int getNotificationsId() {
        return this.notificationsId;
    }
}
