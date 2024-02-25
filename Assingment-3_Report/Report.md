# Computer Graphics
## Assignment 3
### Name: Prakhar Singhal
### Roll Number: 2022111025

---

Image Outputs along with runtimes are present in the `rendered_images` directory as follows.
```
rendered_images/
├── Question-1
│   ├── AntiAliasing_1.png
│   └── AntiAliasing_32.png
├── Question-2
│   ├── scence_1.png
│   ├── scence_2.png
│   ├── scence_3.png
│   └── scence_4.png
└── Question-3
    ├── spp1
    │   ├── big
    │   │   ├── big_0.png
    │   │   ├── big_1.png
    │   │   ├── big_2.png
    │   │   └── big_timing.png
    │   ├── many
    │   │   ├── many_0.png
    │   │   ├── many_1.png
    │   │   ├── many_2.png
    │   │   └── multi_timing.png
    │   ├── med
    │   │   ├── med_0.png
    │   │   ├── med_1.png
    │   │   ├── med_2.png
    │   │   └── med_timing.png
    │   └── small
    │       ├── small_0.png
    │       ├── small_1.png
    │       ├── small_2.png
    │       └── small_timing.png
    ├── spp10
    │   ├── big
    │   │   ├── big_0.png
    │   │   ├── big_1.png
    │   │   ├── big_2.png
    │   │   └── big_timing.png
    │   ├── many
    │   │   ├── many_0.png
    │   │   ├── many_1.png
    │   │   ├── many_2.png
    │   │   └── many_timing.png
    │   ├── med
    │   │   ├── med_0.png
    │   │   ├── med_1.png
    │   │   ├── med_2.png
    │   │   └── med_timing.png
    │   └── small
    │       ├── small_0.png
    │       ├── small_1.png
    │       ├── small_2.png
    │       └── small_timing.png
    ├── spp-100
    │   ├── big
    │   │   ├── big100_0.png
    │   │   ├── big100_1.png
    │   │   └── big_2.png
    │   ├── many
    │   │   ├── many100_0.png
    │   │   ├── many100_1.png
    │   │   └── many100_2.png
    │   ├── med
    │   │   ├── med200_0.png
    │   │   ├── med200_1.png
    │   │   └── med200_2.png
    │   └── small
    │       ├── small_0.png
    │       ├── small_1.png
    │       └── small_2.png
    └── spp1000.png

19 directories, 51 files

```


---

## Questions-1: Why can’t we render point and directional lights with uniform hemisphere sampling or cosine weighted sampling?
**Answer:**
- Rendering point and directional lights with uniform hemisphere sampling or cosine weighted sampling is not feasible due to the following reasons:
    - Delta distributions: Point and directional lights can be represented by delta distributions, which means they have infinitesimal or infinite size respectively. Therefore, using Monte Carlo methods for integration is unnecessary.
    - Infinite distance: In the case of directional lights, the light source is located infinitely far away. As a result, it is impossible for a shadow ray in a sampled direction to hit the light source and illuminate the point.
    - Infinitesimal size: For point lights, the light source is extremely small in size. Similar to directional lights, this makes it impossible for a shadow ray in a sampled direction to hit the light source and illuminate the point.

## Questions-2: Why does the noise increase for the same number of samples in the case of uniform hemisphere and cosine weighted sampling as the size of the area light decreases?
**Answer:**
- The noise increases when using uniform hemisphere and cosine weighted sampling with smaller area lights due to the following factors:
    - Decreased solid angle: As the size of the area light decreases, the solid angle on the hemisphere that the light projects also decreases.
    - Reduced probability of hitting the light: When random directions are sampled from the entire hemisphere, the likelihood of a direction falling within the solid angle required for the shadow ray to hit the area light decreases.
    - Uncolored pixels: With a higher probability of directions not hitting the area light, more pixels remain uncolored after sampling, resulting in increased noise.

---
