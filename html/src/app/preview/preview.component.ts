import { Component, Inject, InjectionToken, Input, OnDestroy, ViewChild } from "@angular/core";
import { Config } from "../app.config";


export const LOCAL_STREAM = new InjectionToken<boolean>('localStream');
@Component({
    selector: 'preview',
    template: require('./preview.html'),
    styles: [require('./preview.scss')]
})
export class PreviewComponent {
    private streamUrl = '/assets/img/testbeeld.png';
    @ViewChild('video') private video: any;

    constructor(private config: Config, @Inject(LOCAL_STREAM) private localStream: boolean) {
    }

    ngOnInit() {
        if (this.localStream) {
            this.initWebcam();
        } else {
            this.streamUrl = this.config.get('stream');
        }
    }

    private initWebcam() {
        let _video = this.video.nativeElement;
        if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
            // TODO allow video options as Input()
            navigator.mediaDevices.getUserMedia({video: {facingMode: "environment"}})
                .then(stream => {
                    _video.srcObject = stream;
                    _video.play();
                })
                .catch((err: MediaStreamError) => {
                    console.warn("Error initializing webcam", err);
                });
        }
    }
}