// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char new_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
//	seq_puts(m, saved_command_line);
	seq_printf(m, "%s\n", new_command_line);
	seq_putc(m, '\n');
	return 0;
}

static void remove_flag(char *cmd, const char *flag)
{
	char *start_addr, *end_addr;
	/* Ensure all instances of a flag are removed */
	while ((start_addr = strstr(cmd, flag))) {
		end_addr = strchr(start_addr, ' ');
		if (end_addr)
			memmove(start_addr, end_addr + 1, strlen(end_addr));
		else
			*(start_addr - 1) = '\0';
	}
}

static void remove_safetynet_flags(char *cmd)
{
	remove_flag(cmd, "androidboot.enable_dm_verity=");
	remove_flag(cmd, "androidboot.secboot=");
	remove_flag(cmd, "androidboot.verifiedbootstate=");
	remove_flag(cmd, "androidboot.veritymode=");
}

static int __init proc_cmdline_init(void)
{
	char *offset_addr;

	strcpy(new_command_line, saved_command_line);
	offset_addr = strstr(new_command_line, "androidboot.mode=reboot");

 	if (offset_addr != NULL)
 		strncpy(offset_addr + 17, "normal", 6);

	remove_safetynet_flags(new_command_line);

	proc_create_single("cmdline", 0, NULL, cmdline_proc_show);
	return 0;
}
fs_initcall(proc_cmdline_init);
