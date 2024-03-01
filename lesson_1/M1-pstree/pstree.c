#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define true ((unsigned char) (1))
#define false ((unsigned char) (0))

unsigned char normal = false, show_pids = false, numberic_sort = false, show_version = false;

typedef struct pidinfo {
	char name[50];		// 进程名称
	pid_t pid;		// 进程id
	pid_t ppid; 		// 父进程id
	struct pidinfo* next;	// 指向下一个进程
}PidInfo;
PidInfo pidinfos[1000];
int pid_count;

int childrens_index[500];

// 定义进程树节点的结构体
typedef struct processtree {
	pid_t pid;	// 进程id
	char name[50];	// 进程名称
	struct childprocesses* children;
}Processtree;
// 定义子进程链表结点的结构体
struct childprocesses {
	struct childprocesses* next;	// 指向下一个子进程的指针
	Processtree* process;		// 指向当前子进程的指针
};

/*
 * 初始化操作
 */
void init() {
	show_pids = false;
	numberic_sort = false;
	show_version = false;
	pid_count = 0;
}
/*
 *  获取命令行参数
 *  params: argc表示命令行参数的数量（包括程序名本身）， argv为命令行参数的具体值
 *  return: 经过处理的参数值
 */
int get_command_line_ops(int argc, char* argv[]) {
	// 目前只处理一个和两个参数值的情况
	assert(argc >= 1 && argc <= 2);
	if (argc == 1) {
		normal = true;
		return 0;
	}
	if (!strcmp(argv[1], "-p") || !strcmp(argv[1], "--show-pids")) {
		show_pids = true;
	} else if (!strcmp(argv[1], "-n") || !strcmp(argv[1], "--numberic-sort")) {
		numberic_sort = true;
	} else if (!strcmp(argv[1], "-V") || !strcmp(argv[1], "--version")) {
		show_version = true;		
	} else {
		return -1;
	}
	assert(!argv[argc]);
	return 0;
}
/*
 * 设置进程ppid和name
 */
void set_process_ppid_pname(PidInfo* node) {
	char* str = (char*)malloc(sizeof(char) * 20);
	if (!str) {
		fprintf(stderr, "分配内存失败\n");
		exit(1);
	}
	// 使用sprintf将进程id转换为字符串
	sprintf(str, "%d", node->pid);

	// 构建进程状态文件路径 (/proc/pid/stat)
	char* processpath = (char*)malloc(sizeof("/proc/") + sizeof(str) + sizeof("/stat"));
	if (!processpath) {
		fprintf(stderr, "分配内存失败\n");
		exit(1);
	}
	strcpy(processpath, "/proc/");
	strcat(processpath, str);
	strcat(processpath, "/stat");
	FILE* fp = fopen(processpath, "r");
	if (fp) {
		char temp;
		pid_t _pid, _ppid;
		char pname[50];
		fscanf(fp, "%d (%s %c %d", &_pid, pname, &temp, &_ppid);
		pname[strlen(pname) - 1] = '\0';
		// 验证进程id是否相等
		assert(node->pid == _pid);
		strcpy(node->name, pname);
		node->ppid = _ppid;
		fclose(fp);
		free(str);
	} else {
		fprintf(stderr, "打开文件失败\n");
		free(str);
		exit(1);
	}
}
/*
 * 设置进程信息
 */
void set_process_info() {
	DIR *dir = opendir("/proc");
	if (!dir) {
		fprintf(stderr, "无法打开 /proc 目录\n");
		exit(EXIT_FAILURE);
	} else {
		int pid = 0;
		struct dirent* entry;
		PidInfo node;
		while ((entry = readdir(dir)) != NULL) {
			// 利用atoi转换，如果不是有效数字返回0
			if ((pid = atoi(entry->d_name)) == 0) {
				continue;	
			}
			node.pid = pid;
			set_process_ppid_pname(&node);
			pidinfos[pid_count++] = node;
		}
	}
}
/*
 * params: pid为父进程ID， index为要保存的子进程索引
 * 查找指定父进程ID的所有紫禁城索引
 */
int find_all_childrens(pid_t pid, int index[]) {
	int count = 0;
	for (int i = 0; i < pid_count; i++) {
		if (pidinfos[i].ppid == pid) {
			index[count++] = i;
		}
	}	
	return count;
}
void create_tree(Processtree* root, int tab_length) {
	char str[100] = {0};
	int childrens_index[500] = {0}; // 所有子进程在pidinfos中的下标
	int count = find_all_childrens(root->pid, childrens_index);
	
	if (!count) { // 叶子结点，没有孩子
		printf("%s(%d)", root->name, root->pid);	// TODO 还有结点，在/proc/pid/task/.../stat中
		return;
	}
	sprintf(str, "%s(%d)-", root->name, root->pid);
	printf("%s", str);

	int flag = 0;
	if (count > 1) {
		printf("+-");
		flag = 2;
	}

	root->children = (struct childprocesses*)malloc(sizeof(struct childprocesses));
	if (root->children == NULL) {
		fprintf(stderr, "分配内存失败\n");
		exit(1);
	}
	struct childprocesses* node = root->children;

	for (int i = 0; i < count; i++) {
		node->process = (Processtree*)malloc(sizeof(Processtree));
		if (!node->process) {
			fprintf(stderr, "分配内存失败\n");
			exit(1);
		}
		node->process->pid = pidinfos[childrens_index[i]].pid;
		strcpy(node->process->name, pidinfos[childrens_index[i]].name);
		create_tree(node->process, strlen(str) + tab_length + flag);
		if (i + 1 < count) {
			node->next = (struct childprocesses*)malloc(sizeof(struct childprocesses));
			if (!node->next) {
				fprintf(stderr, "分配内存失败\n");
				exit(1);
			}	
			node = node->next;
			printf("\n");
			for (int j = 0; j < strlen(str) + tab_length; j++) {
				printf(" ");
			}
			printf("|-");
		}
	}
	free(root);
}
void create_tree_nopid(Processtree* root, int tab_length) {
	char str[100] = {0};
	int childrens_index[500] = {0}; // 所有子进程在pidinfos中的下标
	int count = find_all_childrens(root->pid, childrens_index);
	
	if (!count) { // 叶子结点，没有孩子
		printf("%s", root->name);	// TODO 还有结点，在/proc/pid/task/.../stat中
		return;
	}
	sprintf(str, "%s-", root->name);
	printf("%s", str);

	int flag = 0;
	if (count > 1) {
		printf("+-");
		flag = 2;
	}

	root->children = (struct childprocesses*)malloc(sizeof(struct childprocesses));
	if (root->children == NULL) {
		fprintf(stderr, "分配内存失败\n");
		exit(1);
	}
	struct childprocesses* node = root->children;

	for (int i = 0; i < count; i++) {
		node->process = (Processtree*)malloc(sizeof(Processtree));
		if (!node->process) {
			fprintf(stderr, "分配内存失败\n");
			exit(1);
		}
		node->process->pid = pidinfos[childrens_index[i]].pid;
		strcpy(node->process->name, pidinfos[childrens_index[i]].name);
		create_tree_nopid(node->process, strlen(str) + tab_length + flag);
		if (i + 1 < count) {
			node->next = (struct childprocesses*)malloc(sizeof(struct childprocesses));
			if (!node->next) {
				fprintf(stderr, "分配内存失败\n");
				exit(1);
			}	
			node = node->next;
			printf("\n");
			for (int j = 0; j < strlen(str) + tab_length; j++) {
				printf(" ");
			}
			printf("|-");
		}
	}
	free(root);
}
/*
 * 返回版本信息
 */
char* get_version() {
	char *filename = "version";
	char *version_info = NULL;
	// 文件指针
	FILE *fp = fopen(filename, "r");
	long file_size;
	if (fp) {
		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// 动态分配足够内存
		version_info = (char *)malloc(file_size + 1);

		// 检查内存是否分配成功
		if (version_info == NULL) {
			fprintf(stderr, "内存分配失败\n");
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		fread(version_info, 1, file_size, fp);
		version_info[file_size] = '\0';
		// 关闭文件
		fclose(fp);
		
	} else {
		fprintf(stderr, "无法打开文件: %s\n", filename);
		// 退出程序并返回错误码
		exit(EXIT_FAILURE);
	}
	return version_info;
}
int main(int argc, char *argv[]) {
	/*
	printf("有%d个参数\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
	}
	*/
	init();
	if (get_command_line_ops(argc, argv) == -1) {
		printf("Illegal parameter formats!\n");
		return -1;
	}
	if (normal == true) {
		set_process_info();
		Processtree* root = (Processtree*)malloc(sizeof(Processtree));
		root->pid = pidinfos[0].pid;
		strcpy(root->name, pidinfos[0].name);
		create_tree_nopid(root, 0);
	}
	if (show_pids == true) {
		set_process_info();
		Processtree* root = (Processtree*)malloc(sizeof(Processtree));
		root->pid = pidinfos[0].pid;
		strcpy(root->name, pidinfos[0].name);
		create_tree(root, 0);
	}
	if (numberic_sort == true) {
		set_process_info();
		Processtree* root = (Processtree*)malloc(sizeof(Processtree));
		root->pid = pidinfos[0].pid;
		strcpy(root->name, pidinfos[0].name);
		create_tree_nopid(root, 0);
	}
	if (show_version == true) {
		printf("%s", get_version());	
	}
	return 0;
}
