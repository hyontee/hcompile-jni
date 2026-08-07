package com.blackhub.bronline.game.gui.familysystem;

import android.text.Spanned;
import androidx.compose.runtime.internal.StabilityInferred;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.json.JSONArray;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: GetDataFromJsonAndTransformationToCorrectType.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0002\b\u0002\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0004\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u000e\n\u0000\b\u0007\u0018\u00002\u00020\u0001B\u0005¢\u0006\u0002\u0010\u0002J$\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u00042\b\u0010\u0006\u001a\u0004\u0018\u00010\u00072\f\u0010\b\u001a\b\u0012\u0004\u0012\u00020\t0\u0004J \u0010\n\u001a\b\u0012\u0004\u0012\u00020\u000b0\u00042\b\u0010\f\u001a\u0004\u0018\u00010\u00072\b\u0010\r\u001a\u0004\u0018\u00010\u0007J\u0016\u0010\u000e\u001a\b\u0012\u0004\u0012\u00020\t0\u00042\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007J\u0016\u0010\u000f\u001a\b\u0012\u0004\u0012\u00020\u00100\u00042\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007J\u0016\u0010\u0011\u001a\b\u0012\u0004\u0012\u00020\u00120\u00042\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007¨\u0006\u0013"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/GetDataFromJsonAndTransformationToCorrectType;", "", "()V", "getJsonArrayAndIntCollectionReturnFamilyNotificationList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "thisJsonArray", "Lorg/json/JSONArray;", "thisIntCollection", "", "getJsonArrayReturnFamilyPlayer", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayer;", "jsonArrayWithNick", "jsonArrayWithRankAndStatus", "getJsonArrayReturnIntList", "getJsonArrayReturnSpannedList", "Landroid/text/Spanned;", "getJsonArrayReturnStringList", "", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class GetDataFromJsonAndTransformationToCorrectType {
    public static final int $stable = 0;

    @NotNull
    public final List<Integer> getJsonArrayReturnIntList(@Nullable JSONArray thisJsonArray) {
        ArrayList arrayList = new ArrayList();
        if (thisJsonArray != null) {
            int length = thisJsonArray.length();
            for (int i = 0; i < length; i++) {
                Object obj = thisJsonArray.get(i);
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.Int");
                arrayList.add((Integer) obj);
            }
        }
        return arrayList;
    }

    @NotNull
    public final List<String> getJsonArrayReturnStringList(@Nullable JSONArray thisJsonArray) {
        ArrayList arrayList = new ArrayList();
        if (thisJsonArray != null) {
            int length = thisJsonArray.length();
            for (int i = 0; i < length; i++) {
                Object obj = thisJsonArray.get(i);
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.String");
                arrayList.add((String) obj);
            }
        }
        return arrayList;
    }

    @NotNull
    public final List<Spanned> getJsonArrayReturnSpannedList(@Nullable JSONArray thisJsonArray) {
        ArrayList arrayList = new ArrayList();
        if (thisJsonArray != null) {
            int length = thisJsonArray.length();
            for (int i = 0; i < length; i++) {
                ActionWithString actionWithString = new ActionWithString();
                Object obj = thisJsonArray.get(i);
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.String");
                arrayList.add(actionWithString.convertStringToHtmlType((String) obj));
            }
        }
        return arrayList;
    }

    @NotNull
    public final List<FamilyNotificationData> getJsonArrayAndIntCollectionReturnFamilyNotificationList(@Nullable JSONArray thisJsonArray, @NotNull List<Integer> thisIntCollection) {
        Intrinsics.checkNotNullParameter(thisIntCollection, "thisIntCollection");
        ArrayList arrayList = new ArrayList();
        if (thisJsonArray != null) {
            int i = 0;
            int i2 = 0;
            int i3 = 1;
            while (i < thisJsonArray.length()) {
                Object obj = thisJsonArray.get(i);
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.String");
                Object obj2 = thisJsonArray.get(i3);
                Intrinsics.checkNotNull(obj2, "null cannot be cast to non-null type kotlin.String");
                arrayList.add(new FamilyNotificationData((String) obj, (String) obj2, thisIntCollection.get(i2).intValue()));
                i += 2;
                i3 += 2;
                i2++;
            }
        }
        return arrayList;
    }

    @NotNull
    public final List<FamilyPlayer> getJsonArrayReturnFamilyPlayer(@Nullable JSONArray jsonArrayWithNick, @Nullable JSONArray jsonArrayWithRankAndStatus) {
        ArrayList arrayList = new ArrayList();
        if (jsonArrayWithNick != null && jsonArrayWithRankAndStatus != null && jsonArrayWithNick.length() == jsonArrayWithRankAndStatus.length() / 2) {
            int i = 0;
            int i2 = 1;
            int i3 = 0;
            while (i < jsonArrayWithNick.length()) {
                Object obj = jsonArrayWithNick.get(i);
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.String");
                String str = (String) obj;
                Object obj2 = jsonArrayWithRankAndStatus.get(i3);
                Intrinsics.checkNotNull(obj2, "null cannot be cast to non-null type kotlin.Int");
                int intValue = ((Integer) obj2).intValue();
                Object obj3 = jsonArrayWithRankAndStatus.get(i2);
                Intrinsics.checkNotNull(obj3, "null cannot be cast to non-null type kotlin.Int");
                arrayList.add(new FamilyPlayer(str, intValue, ((Integer) obj3).intValue(), false, 8, null));
                i++;
                i3 += 2;
                i2 += 2;
            }
        }
        return arrayList;
    }
}
