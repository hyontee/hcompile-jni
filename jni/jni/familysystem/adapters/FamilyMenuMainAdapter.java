package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesMenuMainItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuMainAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyMainItemData;
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
/* compiled from: FamilyMenuMainAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000J\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0006\n\u0002\u0018\u0002\n\u0002\b\u0005\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u001dBr\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012S\u0010\u0006\u001aO\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0013\u0012\u00110\r¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\u000e\u0012\u0004\u0012\u00020\u000f\u0018\u00010\u0007j\u0004\u0018\u0001`\u0010\u0012\b\u0010\u0011\u001a\u0004\u0018\u00010\u0012¢\u0006\u0002\u0010\u0013J\b\u0010\u0014\u001a\u00020\u000bH\u0016J\u001c\u0010\u0015\u001a\u00020\u000f2\n\u0010\u0016\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\f\u001a\u00020\u000bH\u0016J\u001c\u0010\u0017\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0018\u001a\u00020\u00192\u0006\u0010\u001a\u001a\u00020\u000bH\u0016J\u000e\u0010\u001b\u001a\u00020\u000f2\u0006\u0010\u001c\u001a\u00020\u000bR\u0010\u0010\u0011\u001a\u0004\u0018\u00010\u0012X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R[\u0010\u0006\u001aO\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0013\u0012\u00110\r¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\u000e\u0012\u0004\u0012\u00020\u000f\u0018\u00010\u0007j\u0004\u0018\u0001`\u0010X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001e"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter$ViewHolder;", "menuList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyMainItemData;", "onClickMenuMainList", "Lkotlin/Function3;", "Lkotlin/ParameterName;", "name", "item", "", "position", "Landroid/view/View;", "mView", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuMainList;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Lkotlin/jvm/functions/Function3;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "getItemCount", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyMenuMainAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final List<FamilyMainItemData> menuList;

    @Nullable
    public final Function3<FamilyMainItemData, Integer, View, Unit> onClickMenuMainList;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyMenuMainAdapter(@NotNull List<FamilyMainItemData> menuList, @Nullable Function3<? super FamilyMainItemData, ? super Integer, ? super View, Unit> function3, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(menuList, "menuList");
        this.menuList = menuList;
        this.onClickMenuMainList = function3;
        this.mainActivity = jNIActivity;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesMenuMainItemBinding inflate = FamiliesMenuMainItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.menuList.get(position), this.onClickMenuMainList);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.menuList.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyMenuMainAdapter.kt */
    @Metadata(d1 = {"\u0000<\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004Jc\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2S\u0010\t\u001aO\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0013\u0012\u00110\u0010¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u0011\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0012R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0013"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMenuMainItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter;Lcom/blackhub/bronline/databinding/FamiliesMenuMainItemBinding;)V", "bind", "", "menuItem", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyMainItemData;", "onClickMenuMainList", "Lkotlin/Function3;", "Lkotlin/ParameterName;", "name", "item", "", "position", "Landroid/view/View;", "mView", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuMainList;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesMenuMainItemBinding binding;
        public final /* synthetic */ FamilyMenuMainAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyMenuMainAdapter familyMenuMainAdapter, FamiliesMenuMainItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyMenuMainAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyMainItemData menuItem, @Nullable final Function3<? super FamilyMainItemData, ? super Integer, ? super View, Unit> onClickMenuMainList) {
            Intrinsics.checkNotNullParameter(menuItem, "menuItem");
            FamiliesMenuMainItemBinding familiesMenuMainItemBinding = this.binding;
            FamilyMenuMainAdapter familyMenuMainAdapter = this.this$0;
            familiesMenuMainItemBinding.menuValue.setText(menuItem.getItemsTitle());
            familiesMenuMainItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuMainAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyMenuMainAdapter.ViewHolder.bind$lambda$2$lambda$0(Function3.this, menuItem, this, view);
                }
            });
            JNIActivity jNIActivity = familyMenuMainAdapter.mainActivity;
            if (jNIActivity != null) {
                if (menuItem.isClicked()) {
                    familiesMenuMainItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_blue_cr5));
                } else {
                    familiesMenuMainItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_white15_cr5));
                }
            }
        }

        public static final void bind$lambda$2$lambda$0(Function3 function3, FamilyMainItemData menuItem, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(menuItem, "$menuItem");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function3 != null) {
                Integer valueOf = Integer.valueOf(this$0.getAdapterPosition());
                Intrinsics.checkNotNull(view);
                function3.invoke(menuItem, valueOf, view);
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.menuList.size();
        for (int i = 0; i < size; i++) {
            if (this.menuList.get(i).isClicked() && i != checkedPosition) {
                this.menuList.get(i).setClicked(false);
                notifyItemChanged(i);
            }
        }
    }
}
