
import jittor as jt
from jittor import init
from jittor import nn
from jittor.dataset.mnist import MNIST
import jittor.transform as transform
import argparse
import os
import numpy as np
import math
import cv2
import time

jt.flags.use_cuda = 1
os.makedirs('images', exist_ok=True)

parser = argparse.ArgumentParser()
parser.add_argument('--n_epochs', type=int, default=200, help='number of epochs of training')
parser.add_argument('--batch_size', type=int, default=64, help='size of the batches')
parser.add_argument('--lr', type=float, default=0.0002, help='adam: learning rate')
parser.add_argument('--b1', type=float, default=0.5, help='adam: decay of first order momentum of gradient')
parser.add_argument('--b2', type=float, default=0.999, help='adam: decay of first order momentum of gradient')
parser.add_argument('--n_cpu', type=int, default=8, help='number of cpu threads to use during batch generation')
parser.add_argument('--latent_dim', type=int, default=100, help='dimensionality of the latent space')
parser.add_argument('--img_size', type=int, default=28, help='size of each image dimension')
parser.add_argument('--channels', type=int, default=1, help='number of image channels')
parser.add_argument('--sample_interval', type=int, default=400, help='interval betwen image samples')
opt = parser.parse_args()
print(opt)
img_shape = (opt.channels, opt.img_size, opt.img_size)

def save_image(img, path, nrow=10):
    N,C,W,H = img.shape
    img2=img.reshape([-1,W*nrow*nrow,H])
    img=img2[:,:W*nrow,:]
    for i in range(1,nrow):
        img=np.concatenate([img,img2[:,W*nrow*i:W*nrow*(i+1),:]],axis=2)
    min_=img.min()
    max_=img.max()
    img=(img-min_)/(max_-min_)*255
    img=img.transpose((1,2,0))
    cv2.imwrite(path,img)
class Generator(nn.Module):

    def __init__(self):
        super(Generator, self).__init__()

        def block(in_feat, out_feat, normalize=True):
            layers = [nn.Linear(in_feat, out_feat)]
            if normalize:
                layers.append(nn.BatchNorm1d(out_feat, 0.8))
            layers.append(nn.LeakyReLU(scale=0.2))
            return layers
        self.model = nn.Sequential(*block(opt.latent_dim, 128, normalize=False), *block(128, 256), *block(256, 512), *block(512, 1024), nn.Linear(1024, int(np.prod(img_shape))), nn.Tanh())

    def execute(self, z):
        img = self.model(z)
        img = img.view((img.shape[0], *img_shape))
        return img

class Discriminator(nn.Module):

    def __init__(self):
        super(Discriminator, self).__init__()
        self.model = nn.Sequential(nn.Linear((opt.img_size ** 2), 512), nn.LeakyReLU(scale=0.2), nn.Linear(512, 256), nn.LeakyReLU(scale=0.2), nn.Linear(256, 1))

    def execute(self, img):
        img_flat = img.view((img.shape[0], (- 1)))
        validity = self.model(img_flat)
        return validity

adversarial_loss = nn.BCELoss()

# Initialize generator and discriminator
generator = Generator()
discriminator = Discriminator()

# Configure data loader
transform = transform.Compose([
    transform.Resize(size=opt.img_size),
    transform.Gray(),
    transform.ImageNormalize(mean=[0.5], std=[0.5]),
])
dataloader = MNIST(train=True, transform=transform).set_attrs(batch_size=opt.batch_size, shuffle=True)

# Optimizers
optimizer_G = jt.optim.Adam(generator.parameters(), lr=opt.lr, betas=(opt.b1, opt.b2))
optimizer_D = jt.optim.Adam(discriminator.parameters(), lr=opt.lr, betas=(opt.b1, opt.b2))

def log(x):
    return jt.log((x + 1e-08))

warmup_times = -1
run_times = 3000
total_time = 0.
cnt = 0

# ----------
#  Training
# ----------

for epoch in range(opt.n_epochs):
    for (i, (real_imgs, _)) in enumerate(dataloader):
        batch_size = real_imgs.shape[0]
        g_target = (1 / (batch_size * 2))
        d_target = (1 / batch_size)

        # -----------------
        #  Train Discriminator
        # -----------------
        
        z = jt.array(np.random.normal(0, 1, (real_imgs.shape[0], opt.latent_dim)).astype(np.float32))
        gen_imgs = generator(z)
        d_real = discriminator(real_imgs)
        d_fake = discriminator(gen_imgs)
        Z = (jt.sum(jt.exp((- d_real))) + jt.sum(jt.exp((- d_fake))))
        d_loss = ((d_target * jt.sum(d_real)) + log(Z))
        optimizer_D.step(d_loss)
        
        # ---------------------
        #  Train Generator
        # ---------------------

        g_loss = ((g_target * (jt.sum(d_real) + jt.sum(d_fake))) + log(Z))
        optimizer_G.step(d_loss+g_loss)
        
        if warmup_times==-1:
            print(('[Epoch %d/%d] [Batch %d/%d] [D loss: %f] [G loss: %f]' % (epoch, opt.n_epochs, i, len(dataloader), d_loss.numpy()[0], g_loss.numpy()[0])))
            batches_done = ((epoch * len(dataloader)) + i)
            if ((batches_done % opt.sample_interval) == 0):
                save_image(gen_imgs.data[:25], ('images/%d.png' % batches_done), nrow=5)
        else:
            jt.sync_all()
            cnt += 1
            print(cnt)
            if cnt == warmup_times:
                jt.sync_all(True)
                sta = time.time()
            if cnt > warmup_times + run_times:
                jt.sync_all(True)
                total_time = time.time() - sta
                print(f"run {run_times} iters cost {total_time} seconds, and avg {total_time / run_times} one iter.")
                exit(0)
