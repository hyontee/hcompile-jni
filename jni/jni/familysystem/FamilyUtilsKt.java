package com.blackhub.bronline.game.gui.familysystem;

import android.graphics.Bitmap;
import android.widget.ImageView;
import androidx.collection.ArrayMap;
import com.blackhub.bronline.game.GameRender;
import com.blackhub.bronline.game.core.JNIActivity;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyUtils.kt */
@Metadata(d1 = {"\u0000(\n\u0000\n\u0002\u0010\u0002\n\u0000\n\u0002\u0010\b\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0007\n\u0002\b\u0003\u001aL\u0010\u0000\u001a\u00020\u00012\u0006\u0010\u0002\u001a\u00020\u00032\u0006\u0010\u0004\u001a\u00020\u00032\u0006\u0010\u0005\u001a\u00020\u00062\u0014\u0010\u0007\u001a\u0010\u0012\u0004\u0012\u00020\u0003\u0012\u0006\u0012\u0004\u0018\u00010\t0\b2\u0006\u0010\n\u001a\u00020\u000b2\u0006\u0010\f\u001a\u00020\u000b2\u0006\u0010\r\u001a\u00020\u000b¨\u0006\u000e"}, d2 = {"renderShopItem", "", "id", "", "modelId", "thisObject", "Landroid/widget/ImageView;", "renderItem", "Landroidx/collection/ArrayMap;", "Landroid/graphics/Bitmap;", "rotX", "", "rotY", "rotZ", "app_siteRelease"}, k = 2, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyUtilsKt {
    public static final void renderShopItem(int i, final int i2, @NotNull final ImageView thisObject, @NotNull final ArrayMap<Integer, Bitmap> renderItem, float f, float f2, float f3) {
        Intrinsics.checkNotNullParameter(thisObject, "thisObject");
        Intrinsics.checkNotNullParameter(renderItem, "renderItem");
        GameRender.getInstance().RequestRender(0, i, i2, 3, 3, f, f2, f3, 0.9f, new GameRender.GameRenderListener() { // from class: com.blackhub.bronline.game.gui.familysystem.FamilyUtilsKt$$ExternalSyntheticLambda0
            @Override // com.blackhub.bronline.game.GameRender.GameRenderListener
            public final void OnRenderComplete(int i3, Bitmap bitmap) {
                FamilyUtilsKt.renderShopItem$lambda$3(thisObject, renderItem, i2, i3, bitmap);
            }
        });
    }

    public static final void renderShopItem$lambda$3(final ImageView thisObject, final ArrayMap renderItem, final int i, int i2, final Bitmap bitmap) {
        Intrinsics.checkNotNullParameter(thisObject, "$thisObject");
        Intrinsics.checkNotNullParameter(renderItem, "$renderItem");
        JNIActivity.getContext().runOnUiThread(new Runnable() { // from class: com.blackhub.bronline.game.gui.familysystem.FamilyUtilsKt$$ExternalSyntheticLambda1
            @Override // java.lang.Runnable
            public final void run() {
                FamilyUtilsKt.renderShopItem$lambda$3$lambda$2$lambda$1(thisObject, bitmap, renderItem, i);
            }
        });
    }

    public static final void renderShopItem$lambda$3$lambda$2$lambda$1(ImageView thisObject, Bitmap bitmap, ArrayMap renderItem, int i) {
        Intrinsics.checkNotNullParameter(thisObject, "$thisObject");
        Intrinsics.checkNotNullParameter(renderItem, "$renderItem");
        thisObject.setImageBitmap(bitmap);
        renderItem.put(Integer.valueOf(i), bitmap);
    }
}
