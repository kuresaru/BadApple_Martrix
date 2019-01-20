package kuresaru.badapple;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;

public class DataGen {

    public static FileOutputStream outputStream;

    public static void main(String[] args) throws Exception {
        File[] imgs = new File("img").listFiles();
        //test1(parseImg(imgs[88]));
        outputStream = new FileOutputStream(new File("badapple.bin"));
        for (File img : imgs) {
            outputStream.write(parseImg(img));
            outputStream.flush();
        }
        outputStream.close();
    }

    /**
     * 把一帧图像转换成32个字节的数据
     *
     * @param img
     * @return
     * @throws Exception
     */
    private static byte[] parseImg(File img) throws Exception {
        BufferedImage image = ImageIO.read(img);
        int[] data = new int[32];
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                data[y] <<= 1;
                data[y] &= 0xFF;
                data[y] |= (image.getRGB(x, y) & 0x00808080) ==
                        0x00808080 ? 1 : 0;
            }
        }
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                data[y + 8] <<= 1;
                data[y + 8] &= 0xFF;
                data[y + 8] |= (image.getRGB(x + 8, y) & 0x00808080) ==
                        0x00808080 ? 1 : 0;
            }
        }
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                data[y + 16] <<= 1;
                data[y + 16] &= 0xFF;
                data[y + 16] |= (image.getRGB(x, y + 8) & 0x00808080) ==
                        0x00808080 ? 1 : 0;
            }
        }
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                data[y + 24] <<= 1;
                data[y + 24] &= 0xFF;
                data[y + 24] |= (image.getRGB(x + 8, y + 8) & 0x00808080) ==
                        0x00808080 ? 1 : 0;
            }
        }
        byte[] b = new byte[data.length];
        for (int i = 0; i < b.length; i++) {
            b[i] = (byte) (data[i] & 0xFF);
        }
        return b;
    }

    /**
     * 把32字节的LED数据打印出来
     *
     * @param a
     */
    private static void test1(byte[] a) {
        for (int i = 0; i < 8; i++)
            System.out.println(test2(a[i], a[i + 8]));
        for (int i = 0; i < 8; i++)
            System.out.println(test2(a[i + 16], a[i + 24]));
    }

    /**
     * 打印一行数据
     *
     * @param a
     * @param b
     * @return
     */
    private static String test2(int a, int b) {
        String c = "00000000" + Integer.toBinaryString(a);
        String d = "00000000" + Integer.toBinaryString(b);
        c = c.substring(c.length() - 8);
        d = d.substring(d.length() - 8);
        //●○
        c = c.replaceAll("1", "●").replaceAll("0", "○");
        d = d.replaceAll("1", "●").replaceAll("0", "○");
        return c + d;
    }

}
