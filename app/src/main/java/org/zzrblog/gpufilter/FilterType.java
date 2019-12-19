package org.zzrblog.gpufilter;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by zzr on 2019/12/19.
 */
public class FilterType {
    // NOTE：这里的ID值要 与
    // cpp/gpufilter/filter/Gpu*****Filter.hpp的getTypeId一一对应
    static final int FILTER_TYPE_NORMAL = 0;
    static final int FILTER_TYPE_CONTRAST = 1;
    static final int FILTER_TYPE_INVERT = 2;

    public List<String> names = new LinkedList<String>();
    public List<Integer> filters = new LinkedList<Integer>();

    public void addFilter(final String name, final int typeId) {
        names.add(name);
        filters.add(typeId);
    }
}
