/*
 * Copyright (C) 2025 404Setup
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

package com.velocitypowered.proxy.command.builtin;

import com.mojang.brigadier.Command;
import com.mojang.brigadier.arguments.StringArgumentType;
import com.mojang.brigadier.tree.LiteralCommandNode;
import com.velocitypowered.api.command.BrigadierCommand;
import com.velocitypowered.api.command.CommandSource;
import com.velocitypowered.api.permission.Tristate;
import com.velocitypowered.api.proxy.Player;
import com.velocitypowered.api.proxy.ProxyServer;
import net.kyori.adventure.text.Component;
import net.kyori.adventure.text.format.NamedTextColor;
import java.util.Optional;

public class KickCommand {
    private static final String KICK_ARG = "kick";

    @SuppressWarnings("checkstyle:MissingJavadocMethod")
    public static BrigadierCommand create(final ProxyServer server) {
        final LiteralCommandNode<CommandSource> node = BrigadierCommand
                .literalArgumentBuilder("vkick")
                .requires(src -> src.getPermissionValue("velocity.command.kick") == Tristate.TRUE)
                .executes(ctx -> {
                    final CommandSource sender = ctx.getSource();
                    sender.sendMessage(Component.translatable(
                            "velocity.command.kick-usage",
                            NamedTextColor.YELLOW));
                    return Command.SINGLE_SUCCESS;
                })
                .then(BrigadierCommand.requiredArgumentBuilder(KICK_ARG, StringArgumentType.word())
                        .suggests((ctx, builder) -> {
                            final String argument = ctx.getArguments().containsKey(KICK_ARG)
                                    ? StringArgumentType.getString(ctx, KICK_ARG)
                                    : "";
                            for (final Player p : server.getAllPlayers()) {
                                final String playerName = p.getUsername();
                                if (playerName.regionMatches(true, 0, argument, 0, argument.length())) {
                                    builder.suggest(playerName);
                                }
                            }
                            return builder.buildFuture();
                        })
                        .executes(ctx -> {
                            final CommandSource sender = ctx.getSource();
                            final String playerName = StringArgumentType.getString(ctx, KICK_ARG);
                            final Optional<Player> player = server.getPlayer(playerName);
                            if (player.isEmpty()) {
                                sender.sendMessage(
                                        Component.translatable(
                                                "velocity.error.not-found-player",
                                                NamedTextColor.RED,
                                                Component.text(playerName)
                                        )
                                );
                            } else {
                                player.get().disconnect(
                                        Component.translatable(
                                                "velocity.kick.default",
                                                NamedTextColor.RED
                                        )
                                );
                            }

                            return Command.SINGLE_SUCCESS;
                        })
                ).build();

        return new BrigadierCommand(node);
    }
}
