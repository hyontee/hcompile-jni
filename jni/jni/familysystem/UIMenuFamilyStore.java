package com.blackhub.bronline.game.gui.familysystem;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesMenuTheStoreOrTheSafeLayoutBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.core.extension.AnyExtensionKt;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuMainAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyMainItemData;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function0;
import kotlin.jvm.functions.Function3;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIMenuFamilyStore.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000t\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\t\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u000b\n\u0002\u0010\u000e\n\u0002\b\u0005\b\u0007\u0018\u00002\u00020\u0001B\u0019\u0012\b\u0010\u0002\u001a\u0004\u0018\u00010\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J\b\u0010\u001e\u001a\u00020\u001aH\u0016J\u001a\u0010\u001f\u001a\u00020\u001a2\u0006\u0010 \u001a\u00020!2\b\u0010\"\u001a\u0004\u0018\u00010#H\u0016J\b\u0010$\u001a\u00020\u001cH\u0016J\b\u0010%\u001a\u00020\u001cH\u0016J\b\u0010&\u001a\u00020\u001cH\u0002J\b\u0010'\u001a\u00020\u001cH\u0002J\u0016\u0010(\u001a\u00020\u001c2\f\u0010)\u001a\b\u0012\u0004\u0012\u00020\f0\u0010H\u0002J\b\u0010*\u001a\u00020\u001cH\u0002J\u0006\u0010+\u001a\u00020\u001cJ\u0014\u0010,\u001a\u00020\u001c2\f\u0010)\u001a\b\u0012\u0004\u0012\u00020\f0\u0010J\u0010\u0010-\u001a\u00020\u001c2\u0006\u0010.\u001a\u00020/H\u0002J\u0010\u00100\u001a\u00020\u001c2\u0006\u00101\u001a\u00020\fH\u0002J\u0014\u00102\u001a\u00020\u001c2\f\u00103\u001a\b\u0012\u0004\u0012\u00020\f0\u0010R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0007\u001a\u0004\u0018\u00010\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\t\u001a\u00020\nX\u0082.¢\u0006\u0002\n\u0000R\u000e\u0010\u000b\u001a\u00020\fX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0002\u001a\u0004\u0018\u00010\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\r\u001a\u0004\u0018\u00010\u000eX\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u000f\u001a\b\u0012\u0004\u0012\u00020\u00110\u0010X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0012\u001a\u00020\u0013X\u0082\u000e¢\u0006\u0002\n\u0000R[\u0010\u0014\u001aO\u0012\u0013\u0012\u00110\u0011¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(\u0018\u0012\u0013\u0012\u00110\f¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(\u0019\u0012\u0013\u0012\u00110\u001a¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(\u001b\u0012\u0004\u0012\u00020\u001c\u0018\u00010\u0015j\u0004\u0018\u0001`\u001dX\u0082\u000e¢\u0006\u0002\n\u0000¨\u00064"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIMenuFamilyStore;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMenuTheStoreOrTheSafeLayoutBinding;", "currentStore", "", "menuAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter;", "menuList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyMainItemData;", "oldTimerForClick", "", "onClickMenuList", "Lkotlin/Function3;", "Lkotlin/ParameterName;", "name", "item", "position", "Landroid/view/View;", "mView", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuMainList;", "getView", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "setDataInMenu", "setDataInView", "setDescriptionForStore", "otherState", "setLogicForClickOnMenuList", "setNullableParameters", "setStartParameters", "setTitle", "title", "", "setVisibleForInfo", "valueOfVisible", "updateParameters", "newParameters", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UIMenuFamilyStore extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;

    @Nullable
    public Animation anim;
    public FamiliesMenuTheStoreOrTheSafeLayoutBinding binding;

    @Nullable
    public final JNIActivity mainActivity;

    @Nullable
    public FamilyMenuMainAdapter menuAdapter;
    public long oldTimerForClick;

    @Nullable
    public Function3<? super FamilyMainItemData, ? super Integer, ? super View, Unit> onClickMenuList;

    @NotNull
    public final List<FamilyMainItemData> menuList = new ArrayList();
    public int currentStore = -1;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIMenuFamilyStore(@Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
    }

    public final void setStartParameters(@NotNull List<Integer> otherState) {
        Intrinsics.checkNotNullParameter(otherState, "otherState");
        if (this.menuList.size() > 0) {
            int size = this.menuList.size();
            int i = 0;
            while (i < size) {
                this.menuList.get(i).setClicked(i == 0);
                FamilyMenuMainAdapter familyMenuMainAdapter = this.menuAdapter;
                if (familyMenuMainAdapter != null) {
                    familyMenuMainAdapter.notifyItemChanged(i);
                }
                i++;
            }
            this.currentStore = this.menuList.get(0).getItemsId();
            setVisibleForInfo(0);
            setTitle(this.menuList.get(0).getItemsTitle());
            FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
            if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesMenuTheStoreOrTheSafeLayoutBinding = null;
            }
            familiesMenuTheStoreOrTheSafeLayoutBinding.buttonGoToTheContents.setVisibility(0);
        }
        if (otherState.size() == 12) {
            ArrayList arrayList = new ArrayList();
            arrayList.add(otherState.get(0));
            arrayList.add(otherState.get(2));
            arrayList.add(otherState.get(4));
            arrayList.add(otherState.get(6));
            arrayList.add(otherState.get(8));
            setDescriptionForStore(arrayList);
        }
    }

    public final void updateParameters(@NotNull List<Integer> newParameters) {
        Intrinsics.checkNotNullParameter(newParameters, "newParameters");
        setDescriptionForStore(newParameters);
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        }
        ConstraintLayout root = familiesMenuTheStoreOrTheSafeLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r5) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesMenuTheStoreOrTheSafeLayoutBinding inflate = FamiliesMenuTheStoreOrTheSafeLayoutBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        if (inflate == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            inflate = null;
        }
        this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
        setDataInMenu();
        setLogicForClickOnMenuList();
        setDataInView();
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding2 = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding2 = null;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding2.buttonGoToTheContents.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyStore$$ExternalSyntheticLambda0
            public /* synthetic */ UIMenuFamilyStore$$ExternalSyntheticLambda0() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyStore.onCreateView$lambda$2(UIMenuFamilyStore.this, view);
            }
        });
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding3 = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding3 = null;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding3.buttonBackToMenu.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyStore$$ExternalSyntheticLambda1
            public /* synthetic */ UIMenuFamilyStore$$ExternalSyntheticLambda1() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyStore.onCreateView$lambda$3(UIMenuFamilyStore.this, view);
            }
        });
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding4 = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding4 = null;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding4.allMoneyValue.setSelected(true);
        familiesMenuTheStoreOrTheSafeLayoutBinding4.materialsValue.setSelected(true);
        familiesMenuTheStoreOrTheSafeLayoutBinding4.masksValue.setSelected(true);
        familiesMenuTheStoreOrTheSafeLayoutBinding4.oilValue.setSelected(true);
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding5 = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding5 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesMenuTheStoreOrTheSafeLayoutBinding = familiesMenuTheStoreOrTheSafeLayoutBinding5;
        }
        ConstraintLayout root = familiesMenuTheStoreOrTheSafeLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public static final void onCreateView$lambda$2(UIMenuFamilyStore this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        Animation animation = this$0.anim;
        if (animation != null) {
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyStore$onCreateView$1$1
                public UIMenuFamilyStore$onCreateView$1$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    ActionsWithJSON actionsWithJSON;
                    int i;
                    actionsWithJSON = UIMenuFamilyStore.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        i = UIMenuFamilyStore.this.currentStore;
                        actionsWithJSON.setClickOnStore(i);
                    }
                }
            });
        }
        view.startAnimation(this$0.anim);
    }

    public static final void onCreateView$lambda$3(UIMenuFamilyStore this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
        if (actionsWithJSON != null) {
            actionsWithJSON.sendGetLayout(5);
        }
    }

    public final void setLogicForClickOnMenuList() {
        this.onClickMenuList = new Function3<FamilyMainItemData, Integer, View, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyStore$setLogicForClickOnMenuList$1
            public UIMenuFamilyStore$setLogicForClickOnMenuList$1() {
            }

            @Override // kotlin.jvm.functions.Function3
            public /* bridge */ /* synthetic */ Unit invoke(FamilyMainItemData familyMainItemData, Integer num, View view) {
                invoke(familyMainItemData, num.intValue(), view);
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilyMainItemData item, int position, @NotNull View mView) {
                long j;
                Animation animation;
                FamilyMenuMainAdapter familyMenuMainAdapter;
                FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding;
                FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding2;
                FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding3;
                FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding4;
                Intrinsics.checkNotNullParameter(item, "item");
                Intrinsics.checkNotNullParameter(mView, "mView");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIMenuFamilyStore.this.oldTimerForClick;
                if (currentTimeMillis - j > 200) {
                    UIMenuFamilyStore.this.oldTimerForClick = System.currentTimeMillis();
                    animation = UIMenuFamilyStore.this.anim;
                    mView.startAnimation(animation);
                    UIMenuFamilyStore.this.currentStore = item.getItemsId();
                    item.setClicked(true);
                    familyMenuMainAdapter = UIMenuFamilyStore.this.menuAdapter;
                    if (familyMenuMainAdapter != null) {
                        familyMenuMainAdapter.notifyItemChanged(position);
                        familyMenuMainAdapter.setCheckOnlyElement(position);
                    }
                    int itemsId = item.getItemsId();
                    FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding5 = null;
                    if (itemsId == 0) {
                        familiesMenuTheStoreOrTheSafeLayoutBinding = UIMenuFamilyStore.this.binding;
                        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
                            Intrinsics.throwUninitializedPropertyAccessException("binding");
                            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
                        }
                        if (familiesMenuTheStoreOrTheSafeLayoutBinding.titleContentsOfTheSafe.getVisibility() == 4) {
                            UIMenuFamilyStore.this.setVisibleForInfo(0);
                        }
                    } else if (itemsId == 1) {
                        familiesMenuTheStoreOrTheSafeLayoutBinding4 = UIMenuFamilyStore.this.binding;
                        if (familiesMenuTheStoreOrTheSafeLayoutBinding4 == null) {
                            Intrinsics.throwUninitializedPropertyAccessException("binding");
                            familiesMenuTheStoreOrTheSafeLayoutBinding4 = null;
                        }
                        if (familiesMenuTheStoreOrTheSafeLayoutBinding4.titleContentsOfTheSafe.getVisibility() == 0) {
                            UIMenuFamilyStore.this.setVisibleForInfo(4);
                        }
                    }
                    UIMenuFamilyStore.this.setTitle(item.getItemsTitle());
                    familiesMenuTheStoreOrTheSafeLayoutBinding2 = UIMenuFamilyStore.this.binding;
                    if (familiesMenuTheStoreOrTheSafeLayoutBinding2 == null) {
                        Intrinsics.throwUninitializedPropertyAccessException("binding");
                        familiesMenuTheStoreOrTheSafeLayoutBinding2 = null;
                    }
                    if (familiesMenuTheStoreOrTheSafeLayoutBinding2.buttonGoToTheContents.getVisibility() == 4) {
                        familiesMenuTheStoreOrTheSafeLayoutBinding3 = UIMenuFamilyStore.this.binding;
                        if (familiesMenuTheStoreOrTheSafeLayoutBinding3 == null) {
                            Intrinsics.throwUninitializedPropertyAccessException("binding");
                        } else {
                            familiesMenuTheStoreOrTheSafeLayoutBinding5 = familiesMenuTheStoreOrTheSafeLayoutBinding3;
                        }
                        familiesMenuTheStoreOrTheSafeLayoutBinding5.buttonGoToTheContents.setVisibility(0);
                    }
                }
            }
        };
    }

    public final void setDataInMenu() {
        List<FamilyMainItemData> list = this.menuList;
        list.clear();
        list.add(new FamilyMainItemData(0, "Сейф", false, 4, null));
        list.add(new FamilyMainItemData(1, "Склад", false, 4, null));
    }

    private final void setDataInView() {
        this.menuAdapter = new FamilyMenuMainAdapter(this.menuList, this.onClickMenuList, this.mainActivity);
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding2 = null;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesMenuTheStoreOrTheSafeLayoutBinding.upgradeList;
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding3 = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesMenuTheStoreOrTheSafeLayoutBinding2 = familiesMenuTheStoreOrTheSafeLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesMenuTheStoreOrTheSafeLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.menuAdapter);
    }

    public final void setVisibleForInfo(int valueOfVisible) {
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding.titleContentsOfTheSafe.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.allMoney.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.allMoneyValue.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.materials.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.materialsValue.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.masks.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.masksValue.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.oil.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.oilValue.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.armor.setVisibility(valueOfVisible);
        familiesMenuTheStoreOrTheSafeLayoutBinding.armorValue.setVisibility(valueOfVisible);
    }

    public final void setTitle(String title) {
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding.storeTitle.setText(title);
        familiesMenuTheStoreOrTheSafeLayoutBinding.titleRightBlock.setText(title);
        TextView textView = familiesMenuTheStoreOrTheSafeLayoutBinding.actionTitle;
        if (!Intrinsics.areEqual(title, "")) {
            JNIActivity jNIActivity = this.mainActivity;
            title = jNIActivity != null ? jNIActivity.getString(R.string.families_store_use_title, title) : null;
        }
        textView.setText(title);
    }

    public final void setDescriptionForStore(List<Integer> otherState) {
        FamiliesMenuTheStoreOrTheSafeLayoutBinding familiesMenuTheStoreOrTheSafeLayoutBinding = this.binding;
        if (familiesMenuTheStoreOrTheSafeLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuTheStoreOrTheSafeLayoutBinding = null;
        }
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity == null || otherState.size() != 5) {
            return;
        }
        familiesMenuTheStoreOrTheSafeLayoutBinding.allMoneyValue.setText(jNIActivity.getString(R.string.common_string_with_ruble, UsefulKt.getPriceWithSpaces(otherState.get(0))));
        familiesMenuTheStoreOrTheSafeLayoutBinding.materialsValue.setText(jNIActivity.getString(R.string.families_store_materials, otherState.get(1)));
        familiesMenuTheStoreOrTheSafeLayoutBinding.masksValue.setText(jNIActivity.getString(R.string.families_store_masks, otherState.get(2)));
        familiesMenuTheStoreOrTheSafeLayoutBinding.oilValue.setText(jNIActivity.getString(R.string.families_store_oil, otherState.get(3)));
        familiesMenuTheStoreOrTheSafeLayoutBinding.armorValue.setText(jNIActivity.getString(R.string.families_store_armor, otherState.get(4)));
    }

    public final void setNullableParameters() {
        this.anim = null;
        this.menuAdapter = null;
        this.onClickMenuList = null;
        this.menuList.clear();
    }
}
