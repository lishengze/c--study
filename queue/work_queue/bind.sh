
echo 63 > /proc/irq/default_smp_affinity

for irq in /proc/irq/*/smp_affinity_list;do
    if [ -f"$irq" ];then
        echo 0-5 > "$irq"
    else
        echo "WARN FILE NOT EXIST $irq" > &2
done


