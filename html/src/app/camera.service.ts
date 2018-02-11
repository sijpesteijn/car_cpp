
import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {HttpClient} from "@angular/common/http";

export interface Dimension {
    name: string;
    width: number;
    height: number;
}

export interface WhiteBalance {
    alpha: number;
    beta: number;
}

export interface CameraSettings {
    dimension?: Dimension;
    captureDelay?: number;
    observersDelay?: number;
    previewDelay?: number;
    whiteBalance?: WhiteBalance;
    hue?: number;
    contrast?: number;
    brightness?: number;
    gain?: number;
    saturation?: number;
    fps?: number;
}

@Injectable()
export class CameraService {
    private cameraSetting: Subject<CameraSettings>;

    constructor(private http: HttpClient, private config: Config) {
        this.cameraSetting = new BehaviorSubject(null);
        this.loadCameraSettings();
    }

    loadCameraSettings() {
        this.http.get(this.config.get('camera.settings'))
            .subscribe(response => {
                this.cameraSetting.next(response as CameraSettings);
            });
    }


    getCameraInfo(): Observable<CameraSettings> {
        return this.cameraSetting;
    }

    updateCameraInfo(cameraSettings: CameraSettings): Observable<CameraSettings> {
        const json = JSON.stringify(cameraSettings);
        return this.http.post(this.config.get('camera.settings'), json).map(response => {
            const cameraSettings = response;
            this.cameraSetting.next(cameraSettings);
            return cameraSettings;
        });
    }
}