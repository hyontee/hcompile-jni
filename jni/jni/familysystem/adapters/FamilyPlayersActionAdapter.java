package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesPlayersActionItemBinding;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayersAction;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyPlayersActionAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000>\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0007\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u0018BS\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012>\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000e¢\u0006\u0002\u0010\u000fJ\b\u0010\u0010\u001a\u00020\u000bH\u0016J\u001c\u0010\u0011\u001a\u00020\r2\n\u0010\u0012\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0013\u001a\u00020\u000bH\u0016J\u001c\u0010\u0014\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0015\u001a\u00020\u00162\u0006\u0010\u0017\u001a\u00020\u000bH\u0016RF\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000eX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0019"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter$ViewHolder;", "playersAction", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayersAction;", "clickActionsButton", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisAction", "", "action", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickActionsButton;", "(Ljava/util/List;Lkotlin/jvm/functions/Function2;)V", "getItemCount", "onBindViewHolder", "holder", "position", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyPlayersActionAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final Function2<FamilyPlayersAction, Integer, Unit> clickActionsButton;

    @NotNull
    public final List<FamilyPlayersAction> playersAction;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyPlayersActionAdapter(@NotNull List<FamilyPlayersAction> playersAction, @Nullable Function2<? super FamilyPlayersAction, ? super Integer, Unit> function2) {
        Intrinsics.checkNotNullParameter(playersAction, "playersAction");
        this.playersAction = playersAction;
        this.clickActionsButton = function2;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesPlayersActionItemBinding inflate = FamiliesPlayersActionItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.playersAction.get(position), this.clickActionsButton);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.playersAction.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyPlayersActionAdapter.kt */
    @Metadata(d1 = {"\u00004\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004JN\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2>\u0010\t\u001a:\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u0007\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u000fR\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0010"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesPlayersActionItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter;Lcom/blackhub/bronline/databinding/FamiliesPlayersActionItemBinding;)V", "bind", "", "action", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayersAction;", "clickActionsButton", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisAction", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickActionsButton;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesPlayersActionItemBinding binding;
        public final /* synthetic */ FamilyPlayersActionAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyPlayersActionAdapter familyPlayersActionAdapter, FamiliesPlayersActionItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyPlayersActionAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyPlayersAction action, @Nullable final Function2<? super FamilyPlayersAction, ? super Integer, Unit> clickActionsButton) {
            Intrinsics.checkNotNullParameter(action, "action");
            final Animation loadAnimation = AnimationUtils.loadAnimation(this.binding.getRoot().getContext(), R.anim.button_click);
            FamiliesPlayersActionItemBinding familiesPlayersActionItemBinding = this.binding;
            familiesPlayersActionItemBinding.actionsTitle.setText(action.getActionsTitle());
            int type = action.getType();
            if (type == 0) {
                familiesPlayersActionItemBinding.buttonPlusAction.setVisibility(0);
                familiesPlayersActionItemBinding.actionsValue.setVisibility(0);
                familiesPlayersActionItemBinding.buttonMinusAction.setVisibility(0);
                if (action.getActionsId() == 2) {
                    familiesPlayersActionItemBinding.actionsValue.setText(String.valueOf(action.getActionsValue() / 60));
                } else {
                    familiesPlayersActionItemBinding.actionsValue.setText(String.valueOf(action.getActionsValue()));
                }
                familiesPlayersActionItemBinding.buttonKick.setVisibility(8);
            } else if (type == 1) {
                familiesPlayersActionItemBinding.buttonPlusAction.setVisibility(4);
                familiesPlayersActionItemBinding.actionsValue.setVisibility(4);
                familiesPlayersActionItemBinding.buttonMinusAction.setVisibility(4);
                familiesPlayersActionItemBinding.buttonKick.setVisibility(0);
            } else {
                familiesPlayersActionItemBinding.buttonPlusAction.setVisibility(4);
                familiesPlayersActionItemBinding.actionsValue.setVisibility(4);
                familiesPlayersActionItemBinding.buttonMinusAction.setVisibility(4);
                familiesPlayersActionItemBinding.buttonKick.setVisibility(8);
                familiesPlayersActionItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter$ViewHolder$$ExternalSyntheticLambda0
                    @Override // android.view.View.OnClickListener
                    public final void onClick(View view) {
                        FamilyPlayersActionAdapter.ViewHolder.bind$lambda$4$lambda$0(loadAnimation, clickActionsButton, action, view);
                    }
                });
            }
            familiesPlayersActionItemBinding.buttonPlusAction.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter$ViewHolder$$ExternalSyntheticLambda1
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyPlayersActionAdapter.ViewHolder.bind$lambda$4$lambda$1(loadAnimation, clickActionsButton, action, view);
                }
            });
            familiesPlayersActionItemBinding.buttonMinusAction.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter$ViewHolder$$ExternalSyntheticLambda2
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyPlayersActionAdapter.ViewHolder.bind$lambda$4$lambda$2(loadAnimation, clickActionsButton, action, view);
                }
            });
            familiesPlayersActionItemBinding.buttonKick.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter$ViewHolder$$ExternalSyntheticLambda3
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyPlayersActionAdapter.ViewHolder.bind$lambda$4$lambda$3(loadAnimation, clickActionsButton, action, view);
                }
            });
        }

        public static final void bind$lambda$4$lambda$0(Animation animation, Function2 function2, FamilyPlayersAction action, View view) {
            Intrinsics.checkNotNullParameter(action, "$action");
            view.startAnimation(animation);
            if (function2 != null) {
                function2.invoke(action, 3);
            }
        }

        public static final void bind$lambda$4$lambda$1(Animation animation, Function2 function2, FamilyPlayersAction action, View view) {
            Intrinsics.checkNotNullParameter(action, "$action");
            view.startAnimation(animation);
            if (function2 != null) {
                function2.invoke(action, 1);
            }
        }

        public static final void bind$lambda$4$lambda$2(Animation animation, Function2 function2, FamilyPlayersAction action, View view) {
            Intrinsics.checkNotNullParameter(action, "$action");
            view.startAnimation(animation);
            if (function2 != null) {
                function2.invoke(action, 0);
            }
        }

        public static final void bind$lambda$4$lambda$3(Animation animation, Function2 function2, FamilyPlayersAction action, View view) {
            Intrinsics.checkNotNullParameter(action, "$action");
            view.startAnimation(animation);
            if (function2 != null) {
                function2.invoke(action, 2);
            }
        }
    }
}
