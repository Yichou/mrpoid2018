package com.edroid.common.utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

/**
 * 频率统计
 * 
 * @author wzp
 * 
 */
public class Frequency {
	// 存放项的出现的次数的Map
	private Map<String, Integer> map = new HashMap<String, Integer>();
	// 对频率进行排序
	private Set<Entiry> set = new TreeSet<Entiry>();
	// 值只进行一次数据转换
	private boolean changeFlag = true;

	/**
	 * 加入统计
	 * 
	 * @param t
	 */
	public void addStatistics(String t) {
		Integer reg = map.get(t);
		map.put(t, reg == null ? 1 : reg + 1);
	}

	/**
	 * 更换数据进行排序
	 */
	private void dataChanged() {
		if (changeFlag) {
			Iterator<String> it = map.keySet().iterator();
			String key = null;

			while (it.hasNext()) {
				key = it.next();
				set.add(new Entiry(key, map.get(key)));
				changeFlag = false;
			}
		}
	}

	/**
	 * 得到最大值的项
	 * 
	 * @return
	 */
	public Entiry getMaxValueItem() {
		if(set.size() == 0)
			return null;
		
		dataChanged();
		Iterator<Entiry> it = set.iterator();
		Entiry fre = null;
		if (it.hasNext()) {
			fre = it.next();
			System.out.println(fre.toString());
		}
		
		return fre;
	}
	
	public int getCount() {
		return set.size();
	}

	public List<Entiry> getDataDesc() {
		dataChanged();
		Iterator<Entiry> it = set.iterator();
		List<Entiry> list = new ArrayList<Entiry>();
		while (it.hasNext()) {
			Entiry fre = it.next();
			list.add(fre);
			System.out.println(fre.toString());
		}
		return list;
	}

	public static void main(String[] args) {
		Frequency f = new Frequency();
		f.addStatistics("w");
		f.addStatistics("w");
		f.addStatistics("w");

		f.addStatistics("a");

		f.addStatistics("z");
		f.addStatistics("z");
		f.addStatistics("z");
		f.addStatistics("z");

		f.addStatistics("c");
		f.addStatistics("c");
		f.addStatistics("c");
		f.addStatistics("c");

		f.getMaxValueItem();
		f.getDataDesc();
	}

	public class Entiry implements Comparable<Entiry> {
		private String key;
		private Integer count;

		public Entiry(String key, Integer count) {
			this.key = key;
			this.count = count;
		}

		public int compareTo(Entiry o) {
			int cmp = count.intValue() - o.count.intValue();
			return (cmp == 0 ? (key).compareTo(o.key) : -cmp);
			// 只需在这儿加一个负号就可以决定是升序还是降序排列 -cmp降序排列，cmp升序排列
			// 因为TreeSet会调用WorkForMap的compareTo方法来决定自己的排序
		}

		@Override
		public String toString() {
			return key + " 出现的次数为：" + count;
		}

		public String getKey() {
			return key;
		}

		public Integer getCount() {
			return count;
		}
	}
}
