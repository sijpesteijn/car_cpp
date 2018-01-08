import {Component} from "@angular/core";
import {Dimension, CameraSettings, CameraService} from "../../camera.service";

@Component({
    selector: 'camera-status',
    template: require('./camera-status.html'),
    styles: [require('./camera-status.scss')]
})
export class CameraStatusComponent {
    private dimensions: Dimension[] = [];
    private cameraSettings: CameraSettings;

    constructor(private cameraService: CameraService) {}

    ngOnInit() {
        this.cameraService.getDimensions().subscribe(resolutions => this.dimensions = resolutions);
        this.cameraService.getCameraInfo().subscribe(cameraSettings => this.cameraSettings = cameraSettings);
    }

    // updatePreviewDelay(event: any) {
    //     this.cameraSettings.previewDelay = event.target.value * 1000;
    //     this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    // }
    //
    // updateCaptureDelay(event: any) {
    //     this.cameraSettings.captureDelay = parseInt(event.target.value, 10);
    //     this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    // }
    //
    // updateObserversDelay(event: any) {
    //     this.cameraSettings.observersDelay = parseInt(event.target.value, 10);
    //     this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    // }
    //
    updateDimensions(event: any) {
        const value = event.target.selectedOptions[0].value
        const dimension = this.dimensions.filter(resolution => resolution.name === value)[0];
        this.cameraSettings.dimension = dimension;
        this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    }

    // updateWhiteBalanceAlpha(event: any) {
    //     this.cameraSettings.whiteBalance.alpha = parseFloat(event.target.value);
    //     this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    // }
    //
    // updateWhiteBalanceBeta(event: any) {
    //     this.cameraSettings.whiteBalance.beta = parseInt(event.target.value, 10);
    //     this.cameraService.updateCameraInfo(this.cameraSettings).subscribe( cameraSettings => this.cameraSettings = cameraSettings);
    // }

    updateCameraInfo() {
        this.cameraService.updateCameraInfo(this.cameraSettings).subscribe(cameraSettings => this.cameraSettings = cameraSettings);
    }

}
