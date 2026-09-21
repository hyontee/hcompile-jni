package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesMenuRankOrColorItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyRankOrColorData;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function3;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyMenuFamilySettingAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000N\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0003\n\u0002\u0010\t\n\u0002\b\u0004\n\u0002\u0018\u0002\n\u0002\b\u0007\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001 Br\u0012\b\u0010\u0003\u001a\u0004\u0018\u00010\u0004\u0012\f\u0010\u0005\u001a\b\u0012\u0004\u0012\u00020\u00070\u0006\u0012S\u0010\b\u001aO\u0012\u0013\u0012\u00110\u0007¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\f\u0012\u0013\u0012\u00110\r¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\u000e\u0012\u0013\u0012\u00110\r¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0010\u0018\u00010\tj\u0004\u0018\u0001`\u0011¢\u0006\u0002\u0010\u0012J\b\u0010\u0013\u001a\u00020\rH\u0016J\u0010\u0010\u0014\u001a\u00020\u00152\u0006\u0010\u000e\u001a\u00020\rH\u0016J\u001c\u0010\u0016\u001a\u00020\u00102\n\u0010\u0017\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u000e\u001a\u00020\rH\u0016J\u001c\u0010\u0018\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0019\u001a\u00020\u001a2\u0006\u0010\u001b\u001a\u00020\rH\u0016J\u000e\u0010\u001c\u001a\u00020\u00102\u0006\u0010\u001d\u001a\u00020\rJ\u000e\u0010\u001e\u001a\u00020\u00102\u0006\u0010\u001f\u001a\u00020\rR\u0010\u0010\u0003\u001a\u0004\u0018\u00010\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R[\u0010\b\u001aO\u0012\u0013\u0012\u00110\u0007¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\f\u0012\u0013\u0012\u00110\r¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\u000e\u0012\u0013\u0012\u00110\r¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0010\u0018\u00010\tj\u0004\u0018\u0001`\u0011X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0005\u001a\b\u0012\u0004\u0012\u00020\u00070\u0006X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006!"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuFamilySettingAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuFamilySettingAdapter$ViewHolder;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "parameters", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyRankOrColorData;", "onClickMenuFamilyList", "Lkotlin/Function3;", "Lkotlin/ParameterName;", "name", "item", "", "position", "clicksType", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuFamilyList;", "(Lcom/blackhub/bronline/game/core/JNIActivity;Ljava/util/List;Lkotlin/jvm/functions/Function3;)V", "getItemCount", "getItemId", "", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "setOnlyStartColor", "newPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyMenuFamilySettingAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final JNIActivity mainActivity;

    @Nullable
    public final Function3<FamilyRankOrColorData, Integer, Integer, Unit> onClickMenuFamilyList;

    @NotNull
    public final List<FamilyRankOrColorData> parameters;

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public long getItemId(int position) {
        return position;
    }

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyMenuFamilySettingAdapter(@Nullable JNIActivity jNIActivity, @NotNull List<FamilyRankOrColorData> parameters, @Nullable Function3<? super FamilyRankOrColorData, ? super Integer, ? super Integer, Unit> function3) {
        Intrinsics.checkNotNullParameter(parameters, "parameters");
        this.mainActivity = jNIActivity;
        this.parameters = parameters;
        this.onClickMenuFamilyList = function3;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesMenuRankOrColorItemBinding inflate = FamiliesMenuRankOrColorItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.parameters.get(position), this.onClickMenuFamilyList);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.parameters.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyMenuFamilySettingAdapter.kt */
    @Metadata(d1 = {"\u00008\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004Jc\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2S\u0010\t\u001aO\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u0010\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0011R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0012"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuFamilySettingAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMenuRankOrColorItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuFamilySettingAdapter;Lcom/blackhub/bronline/databinding/FamiliesMenuRankOrColorItemBinding;)V", "bind", "", "thisItem", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyRankOrColorData;", "onClickMenuFamilyList", "Lkotlin/Function3;", "Lkotlin/ParameterName;", "name", "item", "", "position", "clicksType", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuFamilyList;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesMenuRankOrColorItemBinding binding;
        public final /* synthetic */ FamilyMenuFamilySettingAdapter this$0;

        public static final void bind$lambda$11$lambda$2(View view) {
        }

        public static final void bind$lambda$11$lambda$6(View view) {
        }

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyMenuFamilySettingAdapter familyMenuFamilySettingAdapter, FamiliesMenuRankOrColorItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyMenuFamilySettingAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyRankOrColorData thisItem, @Nullable final Function3<? super FamilyRankOrColorData, ? super Integer, ? super Integer, Unit> onClickMenuFamilyList) {
            String str;
            Intrinsics.checkNotNullParameter(thisItem, "thisItem");
            FamiliesMenuRankOrColorItemBinding familiesMenuRankOrColorItemBinding = this.binding;
            FamilyMenuFamilySettingAdapter familyMenuFamilySettingAdapter = this.this$0;
            int statusType = thisItem.getStatusType();
            if (statusType == 0) {
                familiesMenuRankOrColorItemBinding.valueMenu1.setVisibility(8);
                familiesMenuRankOrColorItemBinding.rankAccessSwitch.setVisibility(8);
                familiesMenuRankOrColorItemBinding.color.setVisibility(8);
                familiesMenuRankOrColorItemBinding.colorsBorder.setVisibility(8);
                familiesMenuRankOrColorItemBinding.ifCurrentColor.setVisibility(8);
                familiesMenuRankOrColorItemBinding.renameRank.setVisibility(0);
                familiesMenuRankOrColorItemBinding.valueMenu2.setText(thisItem.getStatusName());
                JNIActivity jNIActivity = familyMenuFamilySettingAdapter.mainActivity;
                if (jNIActivity != null) {
                    familiesMenuRankOrColorItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_white15_cr5));
                }
                familiesMenuRankOrColorItemBinding.renameRank.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter$ViewHolder$$ExternalSyntheticLambda0
                    @Override // android.view.View.OnClickListener
                    public final void onClick(View view) {
                        FamilyMenuFamilySettingAdapter.ViewHolder.bind$lambda$11$lambda$1(Function3.this, thisItem, this, view);
                    }
                });
                familiesMenuRankOrColorItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter$ViewHolder$$ExternalSyntheticLambda1
                    @Override // android.view.View.OnClickListener
                    public final void onClick(View view) {
                        FamilyMenuFamilySettingAdapter.ViewHolder.bind$lambda$11$lambda$2(view);
                    }
                });
                return;
            }
            if (statusType != 1) {
                if (statusType != 2) {
                    return;
                }
                familiesMenuRankOrColorItemBinding.valueMenu1.setVisibility(8);
                familiesMenuRankOrColorItemBinding.rankAccessSwitch.setVisibility(8);
                familiesMenuRankOrColorItemBinding.renameRank.setVisibility(8);
                familiesMenuRankOrColorItemBinding.color.setVisibility(0);
                familiesMenuRankOrColorItemBinding.colorsBorder.setVisibility(0);
                Boolean ifStartedColor = thisItem.getIfStartedColor();
                if (ifStartedColor != null) {
                    if (ifStartedColor.booleanValue()) {
                        familiesMenuRankOrColorItemBinding.ifCurrentColor.setVisibility(0);
                    } else {
                        familiesMenuRankOrColorItemBinding.ifCurrentColor.setVisibility(4);
                    }
                }
                String thisColor = thisItem.getThisColor();
                if (thisColor != null) {
                    familiesMenuRankOrColorItemBinding.color.setBackgroundColor(Color.parseColor(thisColor));
                }
                familiesMenuRankOrColorItemBinding.valueMenu2.setText(thisItem.getStatusName());
                JNIActivity jNIActivity2 = familyMenuFamilySettingAdapter.mainActivity;
                if (jNIActivity2 != null) {
                    if (thisItem.isClicked() != null && Intrinsics.areEqual(thisItem.isClicked(), Boolean.TRUE)) {
                        familiesMenuRankOrColorItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity2, R.drawable.bg_rectangle_blue_cr5));
                    } else {
                        familiesMenuRankOrColorItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity2, R.drawable.bg_rectangle_white15_cr5));
                    }
                }
                familiesMenuRankOrColorItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter$ViewHolder$$ExternalSyntheticLambda4
                    @Override // android.view.View.OnClickListener
                    public final void onClick(View view) {
                        FamilyMenuFamilySettingAdapter.ViewHolder.bind$lambda$11$lambda$10(Function3.this, thisItem, this, view);
                    }
                });
                return;
            }
            familiesMenuRankOrColorItemBinding.color.setVisibility(8);
            familiesMenuRankOrColorItemBinding.colorsBorder.setVisibility(8);
            familiesMenuRankOrColorItemBinding.ifCurrentColor.setVisibility(8);
            familiesMenuRankOrColorItemBinding.renameRank.setVisibility(8);
            familiesMenuRankOrColorItemBinding.valueMenu1.setVisibility(0);
            familiesMenuRankOrColorItemBinding.rankAccessSwitch.setVisibility(0);
            String statusName = thisItem.getStatusName();
            if (Intrinsics.areEqual(statusName, "складу") ? true : Intrinsics.areEqual(statusName, "сейфу")) {
                str = "Доступ к";
            } else {
                str = "Возможность";
            }
            familiesMenuRankOrColorItemBinding.valueMenu1.setText(str);
            familiesMenuRankOrColorItemBinding.valueMenu2.setText(thisItem.getStatusName());
            JNIActivity jNIActivity3 = familyMenuFamilySettingAdapter.mainActivity;
            if (jNIActivity3 != null) {
                familiesMenuRankOrColorItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity3, R.drawable.bg_rectangle_white15_cr5));
            }
            Integer currentStatus = thisItem.getCurrentStatus();
            if (currentStatus != null) {
                int intValue = currentStatus.intValue();
                if (intValue == 0) {
                    familiesMenuRankOrColorItemBinding.rankAccessSwitch.setChecked(false);
                } else if (intValue == 1) {
                    familiesMenuRankOrColorItemBinding.rankAccessSwitch.setChecked(true);
                }
            }
            familiesMenuRankOrColorItemBinding.rankAccessSwitch.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter$ViewHolder$$ExternalSyntheticLambda2
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyMenuFamilySettingAdapter.ViewHolder.bind$lambda$11$lambda$5(Function3.this, thisItem, this, view);
                }
            });
            familiesMenuRankOrColorItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuFamilySettingAdapter$ViewHolder$$ExternalSyntheticLambda3
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyMenuFamilySettingAdapter.ViewHolder.bind$lambda$11$lambda$6(view);
                }
            });
        }

        public static final void bind$lambda$11$lambda$1(Function3 function3, FamilyRankOrColorData thisItem, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(thisItem, "$thisItem");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function3 != null) {
                function3.invoke(thisItem, Integer.valueOf(this$0.getLayoutPosition()), 0);
            }
        }

        public static final void bind$lambda$11$lambda$5(Function3 function3, FamilyRankOrColorData thisItem, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(thisItem, "$thisItem");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function3 != null) {
                function3.invoke(thisItem, Integer.valueOf(this$0.getLayoutPosition()), 1);
            }
        }

        public static final void bind$lambda$11$lambda$10(Function3 function3, FamilyRankOrColorData thisItem, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(thisItem, "$thisItem");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function3 != null) {
                function3.invoke(thisItem, Integer.valueOf(this$0.getLayoutPosition()), 2);
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.parameters.size();
        for (int i = 0; i < size; i++) {
            if (this.parameters.get(i).isClicked() != null && Intrinsics.areEqual(this.parameters.get(i).isClicked(), Boolean.TRUE) && i != checkedPosition) {
                this.parameters.get(i).setClicked(Boolean.FALSE);
                notifyItemChanged(i);
            }
        }
    }

    public final void setOnlyStartColor(int newPosition) {
        int size = this.parameters.size();
        for (int i = 0; i < size; i++) {
            if (this.parameters.get(i).getIfStartedColor() != null && Intrinsics.areEqual(this.parameters.get(i).getIfStartedColor(), Boolean.TRUE) && i != newPosition) {
                this.parameters.get(i).setIfStartedColor(Boolean.FALSE);
                notifyItemChanged(i);
            }
        }
    }
}
