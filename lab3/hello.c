#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Yevhen Seliutin <evgenkoli8@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

// Оголосити структуру для елемента списку
struct hello_entry {
    struct list_head list;
    ktime_t time;
};

// Створити статичну змінну голови списку
static LIST_HEAD(hello_list);

// Оголосити параметр
static unsigned int print_count = 1;
module_param(print_count, uint, S_IRUGO);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!' (default=1)");

static int __init hello_init(void)
{
    if (print_count == 0 || (print_count > 5 && print_count < 10)) {
        // Надрукувати попередження та продовжити роботу
        pr_warn("Print count is 0 or between 5 and 10. Defaulting to 1.\n");
    } else if (print_count > 10) {
        // Надрукувати повідомлення про помилку та повернути -EINVAL
        pr_err("Print count is greater than 10. Module cannot be loaded.\n");
        return -EINVAL;
    }

    // Виділити пам'ять та додати елементи до списку відповідно до значень print_count
    while (print_count > 0) {
        struct hello_entry *entry = kmalloc(sizeof(*entry), GFP_KERNEL);

        if (!entry) {
            pr_err("Failed to allocate memory for hello_entry\n");
            return -ENOMEM;
        }

        // Зберегти поточний час ядра
        entry->time = ktime_get();

        // Додати елемент до списку
        list_add(&entry->list, &hello_list);

        // Надрукувати привітання
        pr_emerg("Hello, world!\n");

        print_count--;
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_entry *entry, *temp;

    // Пройти по списку
    list_for_each_entry_safe(entry, temp, &hello_list, list) {
        // Надрукувати час події в наносекундах
        pr_emerg("Time: %lld ns\n", ktime_to_ns(entry->time));

        // Вилучити елемент зі списку
        list_del(&entry->list);

        // Звільнити виділену пам'ять
        kfree(entry);
    }
}

module_init(hello_init);
module_exit(hello_exit);
