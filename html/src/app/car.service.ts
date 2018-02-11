
import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {HttpClient} from "@angular/common/http";

export interface Car {
    throttle: number;
    angle: number;
    mode: number;
    enabled: boolean;
}

@Injectable()
export class CarService {
    private car: Subject<Car>;

    constructor(private http: HttpClient, private config: Config) {
        this.car = new BehaviorSubject({throttle: 0, angle: 0, mode: 0, enabled: false});
    }

    getCar(): Observable<Car> {
        return this.car;
    }

    updateCar(car: Car) {
        this.car.next(car);
    }

    saveCar(car: Car)  {
        return (this.http.post(this.config.get('car.save'), JSON.stringify(car)))
            .map((car: Car) => {
                this.car.next(car);
            });
    }
}