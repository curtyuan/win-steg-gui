import argparse
import cv2
import numpy as np
from sewar.full_ref import mse, psnr, ssim
import os

class ImageInfo:
    def __init__(self, image_path):
        self.image_path = image_path
        self.image = cv2.imread(image_path)
        if self.image is None:
            raise ValueError(f"Could not load image: {image_path}")
    
    def get_basic_info(self):
        """Get basic information about the image without comparison"""
        height, width, channels = self.image.shape
        file_size = os.path.getsize(self.image_path)
        bit_depth = str(self.image.dtype)
        
        return {
            'width': width,
            'height': height,
            'channels': channels,
            'file_size': file_size,
            'bit_depth': bit_depth
        }
        
    def calculate_metrics(self, reference_image_path):
        """Calculate comparison metrics against a reference image"""
        reference_image = cv2.imread(reference_image_path)
        if reference_image is None:
            raise ValueError(f"Could not load reference image: {reference_image_path}")
            
        # Ensure images have same dimensions
        if self.image.shape != reference_image.shape:
            raise ValueError("Images must have the same dimensions")
        
        # Calculate comparison metrics
        mse_value = mse(self.image, reference_image)
        psnr_value = psnr(self.image, reference_image)
        ssim_value = float(ssim(self.image, reference_image)[0])
        
        # Get basic info
        basic_info = self.get_basic_info()
        
        # Combine basic info with comparison metrics
        basic_info.update({
            'mse': mse_value,
            'psnr': psnr_value,
            'ssim': ssim_value
        })
        
        return basic_info

def print_image_info(info, is_reference=False):
    """Helper function to print image information"""
    print(f"Dimensions: {info['width']} x {info['height']} pixels")
    print(f"Channels: {info['channels']}")
    print(f"Bit Depth: {info['bit_depth']}")
    print(f"File Size: {info['file_size']:,} bytes")
    
    # Only print comparison metrics if not reference image
    if not is_reference and 'mse' in info:
        print("\nComparison Metrics (against reference):")
        print(f"MSE: {info['mse']:.6f}")
        print(f"PSNR: {info['psnr']:.2f} dB")
        print(f"SSIM: {info['ssim']:.6f}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Calculate image quality metrics')
    parser.add_argument('reference_image', type=str, help='Path to the reference image')
    parser.add_argument('image_paths', type=str, nargs='+', help='Path(s) to image(s) to analyze')
    args = parser.parse_args()
    
    # First show information about reference image
    try:
        print("\nReference Image:")
        print(f"Path: {args.reference_image}")
        print("-" * 50)
        ref_image = ImageInfo(args.reference_image)
        ref_info = ref_image.get_basic_info()
        print_image_info(ref_info, is_reference=True)
    except Exception as e:
        print(f"Error processing reference image: {str(e)}")
        exit(1)
    
    # Then process each comparison image
    for image_path in args.image_paths:
        try:
            print(f"\nAnalyzing Image:")
            print(f"Path: {image_path}")
            print("-" * 50)
            
            image_info = ImageInfo(image_path)
            metrics = image_info.calculate_metrics(args.reference_image)
            print_image_info(metrics)
            
        except Exception as e:
            print(f"Error processing {image_path}: {str(e)}")
            continue
