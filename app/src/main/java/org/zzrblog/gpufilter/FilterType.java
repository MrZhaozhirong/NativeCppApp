package org.zzrblog.gpufilter;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by zzr on 2019/12/19.
 */
public class FilterType {
    // NOTE：这里的ID值要 与
    // cpp/gpufilter/filter/Gpu*****Filter.hpp的getTypeId一一对应
    static final int FILTER_TYPE_NORMAL = 0x1010;
    static final int FILTER_TYPE_COLOR_INVERT = 0x1011;
    static final int FILTER_TYPE_PIXELATION = 0x1012;
    static final int FILTER_TYPE_CONTRAST = 0x1013;
    static final int FILTER_TYPE_BRIGHTNESS = 0x1014;
    static final int FILTER_TYPE_HUE = 0x1015;
    static final int FILTER_TYPE_EXPOSURE = 0x1016;
    static final int FILTER_TYPE_SATURATION = 0x1017;

    public List<String> names = new LinkedList<String>();
    public List<Integer> filters = new LinkedList<Integer>();

    public void addFilter(final String name, final int typeId) {
        names.add(name);
        filters.add(typeId);
    }
}
