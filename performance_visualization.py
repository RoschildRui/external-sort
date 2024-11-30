import matplotlib.pyplot as plt
import numpy as np

# 数据
times = [763, 753]  # 排序时间（秒）
labels = ['Before\nOptimization', 'After\nOptimization']  # 添加换行使标签更清晰
improvement = 763 - 753
improvement_percentage = (improvement / 763) * 100

# 创建更大的图形
plt.figure(figsize=(15, 6))

# 1. 柱状图
plt.subplot(121)
bars = plt.bar(labels, times, color=['#FF9999', '#66B2FF'], width=0.6)
plt.title('External Sort Performance Comparison\n(4GB Data, 2GB Memory)', pad=20)
plt.ylabel('Time (seconds)')

# 调整数值标签的位置
for bar in bars:
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height + 5,  # 向上移动5个单位
             f'{int(height)}s',
             ha='center', va='bottom')

# 2. 优化效果饼图
plt.subplot(122)
optimization_data = [improvement, 753]
colors = ['#98FB98', '#66B2FF']

# 调整饼图的标签位置
plt.pie(optimization_data, 
        labels=[f'Improvement\n({improvement}s)', 
                f'Optimized Time\n({753}s)'],
        colors=colors,
        autopct='%1.1f%%',
        startangle=90,
        labeldistance=1.1,  # 增加标签距离
        pctdistance=0.75)   # 调整百分比标签位置

plt.title('Optimization Effect Analysis', pad=20)

# 将总体说明移到底部，并增加间距
plt.figtext(0.5, -0.05, 
            f'Total Improvement: {improvement} seconds ({improvement_percentage:.1f}%)\n'
            'Memory Usage: 2GB, Data Size: 4GB',
            ha='center', fontsize=10,
            bbox=dict(facecolor='white', alpha=0.8, edgecolor='none', pad=5))

# 调整布局，增加子图之间的间距
plt.tight_layout(pad=3.0)

# 保存图表，增加边距
plt.savefig('optimization_comparison.png', 
            dpi=300, 
            bbox_inches='tight',
            pad_inches=0.5)
plt.close() 