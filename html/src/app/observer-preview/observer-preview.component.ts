import {Component, Input, OnDestroy} from "@angular/core";
import {CameraService, CameraSettings} from "../camera.service";

@Component({
    selector: 'observer-preview',
    template: require('./observer-preview.html'),
    styles: [require('./observer-preview.scss')]
})
export class ObserverPreviewComponent implements OnDestroy {
    private reloadInterval: any;
    private last_update: Date;
    private status: string = 'loading';
    private cameraSettings: CameraSettings;
    private interval: number = 5 * 1000;
    private streamDefault = '/assets/img/testbeeld.png';
    private streamUrl = this.streamDefault;
    @Input()
    private baseUrl = '/camera/snapshot/';
    @Input()
    private height: number = 360;

    constructor(private cameraService: CameraService) {
        this.cameraService.getCameraInfo().subscribe(cameraSettings => {
            if (cameraSettings) {
                this.cameraSettings = cameraSettings;
                this.interval = cameraSettings.previewDelay;
                if (this.reloadInterval) {
                    clearInterval(this.reloadInterval);
                }
                setTimeout(() => {
                    this.reloadPreview();
                }, 300);
            }
        });
    }

    ngAfterViewInit() {
        this.getPreviewImage();
    }

    ngOnDestroy() {
        setTimeout(() => {
            clearInterval(this.reloadInterval);
        }, 200);
    }

    private reloadPreview() {
        this.reloadInterval = setInterval(() => {
            this.getPreviewImage();
        }, (this.interval));
    }

    private getPreviewImage() {
        this.last_update = new Date();
        const url = this.baseUrl + this.last_update;
        const img = new Image();
        img.onload = () => {
            this.status = 'loaded';
            this.streamUrl = url;
        };
        img.onerror = () => {
            this.status = 'error';
            this.streamUrl = this.streamDefault;
        };
        img.src = url;
    }
}