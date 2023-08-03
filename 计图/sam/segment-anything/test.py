import numpy as np
import cv2 as cv2
from segment_anything import SamAutomaticMaskGenerator, sam_model_registry
# 权重路径
sam = sam_model_registry["vit_b"](checkpoint="./sam_vit_b_01ec64.pth")
mask_generator = SamAutomaticMaskGenerator(sam)
image = cv2.imread("./person.jpg")
image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
masks = mask_generator.generate(image)
# 这里只取了第一个掩码，他会返回一个列表，“不同于nlp返回一个结果，sam会返回多个近似值”
mask = (masks[1]["segmentation"] * 255.0).astype(np.uint8)
mask = np.array(mask)

cv2.namedWindow("image", cv2.WINDOW_NORMAL)
cv2.namedWindow("mask", cv2.WINDOW_NORMAL)
cv2.imshow("image", image)
cv2.imshow("mask", mask)
cv2.imwrite("mask.jpg", mask)
cv2.waitKey(0)
