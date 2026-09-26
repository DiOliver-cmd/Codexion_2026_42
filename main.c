/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                            +:+      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dilferre <dilferre@student.42sp.org.br>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 10:00:00 by dilferre  #+#    #+#             */
/*   Updated: 2026/09/25 10:00:00 by dilferre  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		monitor;

	memset(&sim, 0, sizeof(t_simulation));
	parse_args(argc, argv, &sim);
	if (!validate_args(&sim))
	{
		log_error("Invalid arguments: all values must be positive");
		return (1);
	}
	init_simulation(&sim);
	start_coders(&sim);
	start_monitor(&sim, &monitor);
	wait_for_completion(&sim, monitor);
	cleanup_simulation(&sim);
	return (0);
}
