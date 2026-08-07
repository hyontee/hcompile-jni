package com.blackhub.bronline.game.gui.familysystem;

import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesTopPresentsLayoutBinding;
import com.blackhub.bronline.game.GameRender;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyTopPresentsAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPresent;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemList;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemRewardTopObj;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UITopPresents.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000r\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0003\n\u0002\u0010\b\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0005\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\u00020\u0001B\u0017\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J\b\u0010\u0016\u001a\u00020\u0017H\u0016J\u001a\u0010\u0018\u001a\u00020\u00172\u0006\u0010\u0019\u001a\u00020\u001a2\b\u0010\u001b\u001a\u0004\u0018\u00010\u001cH\u0016J\b\u0010\u001d\u001a\u00020\u001eH\u0016J\b\u0010\u001f\u001a\u00020\u001eH\u0016J \u0010 \u001a\u00020\u001e2\u0006\u0010!\u001a\u00020\"2\u0006\u0010#\u001a\u00020\"2\u0006\u0010$\u001a\u00020%H\u0002J\u0016\u0010&\u001a\b\u0012\u0004\u0012\u00020\u00110\u00102\u0006\u0010'\u001a\u00020\"H\u0002J\b\u0010(\u001a\u00020\u001eH\u0002J\u0018\u0010)\u001a\u00020\u001e2\u0006\u0010*\u001a\u00020\"2\u0006\u0010$\u001a\u00020+H\u0002J\u0006\u0010,\u001a\u00020\u001eJ\u0006\u0010-\u001a\u00020\u001eR\u0010\u0010\u0007\u001a\u0004\u0018\u00010\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\n\u001a\u0004\u0018\u00010\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u000b\u001a\u0004\u0018\u00010\fX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\r\u001a\u00020\u000eX\u0082.¢\u0006\u0002\n\u0000R\u0014\u0010\u000f\u001a\b\u0012\u0004\u0012\u00020\u00110\u0010X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0012\u001a\b\u0012\u0004\u0012\u00020\u00110\u0010X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0013\u001a\b\u0012\u0004\u0012\u00020\u00110\u0010X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0014\u001a\u0004\u0018\u00010\u0015X\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006."}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UITopPresents;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "adapterTop1", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyTopPresentsAdapter;", "adapterTop2", "adapterTop3", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesTopPresentsLayoutBinding;", "listPresents1", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPresent;", "listPresents2", "listPresents3", "thisFamilySystemList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemList;", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "", "onLayoutShown", "renderAccessories", "id", "", "modelId", "thisObject", "Landroid/widget/ImageView;", "setCollectionsWithPresents", "currentTop", "setDataInView", "setDaysInView", "allDays", "Landroid/widget/TextView;", "setNullableParameters", "setParameters", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UITopPresents extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public FamilyTopPresentsAdapter adapterTop1;

    @Nullable
    public FamilyTopPresentsAdapter adapterTop2;

    @Nullable
    public FamilyTopPresentsAdapter adapterTop3;

    @Nullable
    public Animation anim;
    public FamiliesTopPresentsLayoutBinding binding;

    @NotNull
    public final List<FamilyPresent> listPresents1;

    @NotNull
    public final List<FamilyPresent> listPresents2;

    @NotNull
    public final List<FamilyPresent> listPresents3;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;

    @Nullable
    public FamilySystemList thisFamilySystemList;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UITopPresents(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.listPresents1 = new ArrayList();
        this.listPresents2 = new ArrayList();
        this.listPresents3 = new ArrayList();
        this.thisFamilySystemList = mainRoot.getFamilySystemListFromJson();
    }

    public final void setParameters() {
        List<FamilyPresent> list = this.listPresents1;
        list.clear();
        list.addAll(setCollectionsWithPresents(0));
        List<FamilyPresent> list2 = this.listPresents2;
        list2.clear();
        list2.addAll(setCollectionsWithPresents(1));
        List<FamilyPresent> list3 = this.listPresents3;
        list3.clear();
        list3.addAll(setCollectionsWithPresents(2));
        setDataInView();
        FamilySystemList familySystemList = this.thisFamilySystemList;
        if (familySystemList != null) {
            int accessoriesObjectId = familySystemList.getRewardTopList().get(0).getAccessoriesObjectId();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding = this.binding;
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding2 = null;
            if (familiesTopPresentsLayoutBinding == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesTopPresentsLayoutBinding = null;
            }
            ImageView accessories1Item = familiesTopPresentsLayoutBinding.accessories1Item;
            Intrinsics.checkNotNullExpressionValue(accessories1Item, "accessories1Item");
            renderAccessories(0, accessoriesObjectId, accessories1Item);
            int accessoriesObjectId2 = familySystemList.getRewardTopList().get(1).getAccessoriesObjectId();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding3 = this.binding;
            if (familiesTopPresentsLayoutBinding3 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesTopPresentsLayoutBinding3 = null;
            }
            ImageView accessories2Item = familiesTopPresentsLayoutBinding3.accessories2Item;
            Intrinsics.checkNotNullExpressionValue(accessories2Item, "accessories2Item");
            renderAccessories(1, accessoriesObjectId2, accessories2Item);
            int accessoriesObjectId3 = familySystemList.getRewardTopList().get(2).getAccessoriesObjectId();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding4 = this.binding;
            if (familiesTopPresentsLayoutBinding4 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesTopPresentsLayoutBinding4 = null;
            }
            ImageView accessories3Item = familiesTopPresentsLayoutBinding4.accessories3Item;
            Intrinsics.checkNotNullExpressionValue(accessories3Item, "accessories3Item");
            renderAccessories(2, accessoriesObjectId3, accessories3Item);
            int accessoriesTime = familySystemList.getRewardTopList().get(0).getAccessoriesTime();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding5 = this.binding;
            if (familiesTopPresentsLayoutBinding5 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesTopPresentsLayoutBinding5 = null;
            }
            TextView accessories1ValueDays = familiesTopPresentsLayoutBinding5.accessories1ValueDays;
            Intrinsics.checkNotNullExpressionValue(accessories1ValueDays, "accessories1ValueDays");
            setDaysInView(accessoriesTime, accessories1ValueDays);
            int accessoriesTime2 = familySystemList.getRewardTopList().get(1).getAccessoriesTime();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding6 = this.binding;
            if (familiesTopPresentsLayoutBinding6 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesTopPresentsLayoutBinding6 = null;
            }
            TextView accessories2ValueDays = familiesTopPresentsLayoutBinding6.accessories2ValueDays;
            Intrinsics.checkNotNullExpressionValue(accessories2ValueDays, "accessories2ValueDays");
            setDaysInView(accessoriesTime2, accessories2ValueDays);
            int accessoriesTime3 = familySystemList.getRewardTopList().get(2).getAccessoriesTime();
            FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding7 = this.binding;
            if (familiesTopPresentsLayoutBinding7 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
            } else {
                familiesTopPresentsLayoutBinding2 = familiesTopPresentsLayoutBinding7;
            }
            TextView accessories3ValueDays = familiesTopPresentsLayoutBinding2.accessories3ValueDays;
            Intrinsics.checkNotNullExpressionValue(accessories3ValueDays, "accessories3ValueDays");
            setDaysInView(accessoriesTime3, accessories3ValueDays);
        }
    }

    public final List<FamilyPresent> setCollectionsWithPresents(int currentTop) {
        ArrayList arrayList = new ArrayList();
        FamilySystemList familySystemList = this.thisFamilySystemList;
        if (familySystemList != null) {
            FamilySystemRewardTopObj familySystemRewardTopObj = familySystemList.getRewardTopList().get(currentTop);
            if (familySystemRewardTopObj.getAdditionalCar() != 0) {
                arrayList.add(new FamilyPresent("— доп. авто слот:", "+" + familySystemRewardTopObj.getAdditionalCar()));
            }
            if (familySystemRewardTopObj.getMoneyReward() != 0) {
                arrayList.add(new FamilyPresent("— баланс:", familySystemRewardTopObj.getMoneyReward() + "₽"));
            }
            if (familySystemRewardTopObj.getScoreReward() != 0) {
                arrayList.add(new FamilyPresent("— семейных монет:", "+" + familySystemRewardTopObj.getScoreReward()));
            }
            if (familySystemRewardTopObj.getTokenReward() != 0) {
                arrayList.add(new FamilyPresent("— жетонов:", "+" + familySystemRewardTopObj.getTokenReward()));
            }
        }
        return arrayList;
    }

    private final void setDataInView() {
        this.adapterTop1 = new FamilyTopPresentsAdapter(this.listPresents1);
        this.adapterTop2 = new FamilyTopPresentsAdapter(this.listPresents2);
        this.adapterTop3 = new FamilyTopPresentsAdapter(this.listPresents3);
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding = this.binding;
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding2 = null;
        if (familiesTopPresentsLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesTopPresentsLayoutBinding.presents1;
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding3 = this.binding;
        if (familiesTopPresentsLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding3 = null;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesTopPresentsLayoutBinding3.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.adapterTop1);
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding4 = this.binding;
        if (familiesTopPresentsLayoutBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding4 = null;
        }
        RecyclerView recyclerView2 = familiesTopPresentsLayoutBinding4.presents2;
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding5 = this.binding;
        if (familiesTopPresentsLayoutBinding5 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding5 = null;
        }
        recyclerView2.setLayoutManager(new LinearLayoutManager(familiesTopPresentsLayoutBinding5.getRoot().getContext(), 1, false));
        recyclerView2.setAdapter(this.adapterTop2);
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding6 = this.binding;
        if (familiesTopPresentsLayoutBinding6 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding6 = null;
        }
        RecyclerView recyclerView3 = familiesTopPresentsLayoutBinding6.presents3;
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding7 = this.binding;
        if (familiesTopPresentsLayoutBinding7 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesTopPresentsLayoutBinding2 = familiesTopPresentsLayoutBinding7;
        }
        recyclerView3.setLayoutManager(new LinearLayoutManager(familiesTopPresentsLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView3.setAdapter(this.adapterTop3);
    }

    public final void renderAccessories(int id, int modelId, ImageView thisObject) {
        GameRender.getInstance().RequestRender(0, id, modelId, 3, 3, 20.0f, 180.0f, 0.0f, 0.9f, new GameRender.GameRenderListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UITopPresents$$ExternalSyntheticLambda1
            public final /* synthetic */ ImageView f$1;

            public /* synthetic */ UITopPresents$$ExternalSyntheticLambda1(ImageView thisObject2) {
                r2 = thisObject2;
            }

            @Override // com.blackhub.bronline.game.GameRender.GameRenderListener
            public final void OnRenderComplete(int i, Bitmap bitmap) {
                UITopPresents.renderAccessories$lambda$12(UITopPresents.this, r2, i, bitmap);
            }
        });
    }

    public static final void renderAccessories$lambda$12(UITopPresents this$0, ImageView thisObject, int i, Bitmap bitmap) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        Intrinsics.checkNotNullParameter(thisObject, "$thisObject");
        JNIActivity jNIActivity = this$0.mainActivity;
        if (jNIActivity != null) {
            jNIActivity.runOnUiThread(new Runnable() { // from class: com.blackhub.bronline.game.gui.familysystem.UITopPresents$$ExternalSyntheticLambda0
                public final /* synthetic */ ImageView f$1;
                public final /* synthetic */ Bitmap f$2;

                public /* synthetic */ UITopPresents$$ExternalSyntheticLambda0(ImageView thisObject2, Bitmap bitmap2) {
                    r2 = thisObject2;
                    r3 = bitmap2;
                }

                @Override // java.lang.Runnable
                public final void run() {
                    UITopPresents.renderAccessories$lambda$12$lambda$11$lambda$10(UITopPresents.this, r2, r3);
                }
            });
        }
    }

    public static final void renderAccessories$lambda$12$lambda$11$lambda$10(UITopPresents this_run, ImageView thisObject, Bitmap bitmap) {
        Intrinsics.checkNotNullParameter(this_run, "$this_run");
        Intrinsics.checkNotNullParameter(thisObject, "$thisObject");
        thisObject.setImageBitmap(bitmap);
    }

    public final void setDaysInView(int allDays, TextView thisObject) {
        String str;
        if (allDays < 5) {
            str = allDays + " дня";
        } else {
            str = allDays + " дней";
        }
        thisObject.setText(str);
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding = this.binding;
        if (familiesTopPresentsLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesTopPresentsLayoutBinding = null;
        }
        ConstraintLayout root = familiesTopPresentsLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r4) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesTopPresentsLayoutBinding inflate = FamiliesTopPresentsLayoutBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding = null;
        if (inflate == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            inflate = null;
        }
        this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
        FamiliesTopPresentsLayoutBinding familiesTopPresentsLayoutBinding2 = this.binding;
        if (familiesTopPresentsLayoutBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesTopPresentsLayoutBinding = familiesTopPresentsLayoutBinding2;
        }
        ConstraintLayout root = familiesTopPresentsLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public final void setNullableParameters() {
        this.anim = null;
        this.adapterTop1 = null;
        this.adapterTop2 = null;
        this.adapterTop3 = null;
        this.thisFamilySystemList = null;
        this.listPresents1.clear();
        this.listPresents2.clear();
        this.listPresents3.clear();
    }
}
