import React, { Component } from 'react';
import Main from "./sections/Main.js"
import MyImages from "./sections/MyImages.js"
import OtherImages from "./sections/OtherImages"
import DownloadedImages from "./sections/DownloadedImages"
import RequestList from "./sections/RequestList.js"
import FriendsBar from "./sections/FriendsBar.js"
import "./css/main.css"

class App extends Component {
	constructor(props) {
		super(props);

		this.state = {
			selscreen: 1
		}
	}

	setUser = (u) => {
		this.setState({selscreen: 0, seluser: u});
	}

	setScreen = (i) => {
		this.setState({selscreen: i});
	}

	render() {
		let s = this.state.selscreen;

		let my_images = [
			{url: "1.jpg", views: 256},
			{url: "2.jpg", views: 123},
			{url: "3.jpg", views: 4124},
			{url: "4.jpg", views: 421},
			{url: "5.jpg", views: 94},
			{url: "1.jpg", views: 3},
			{url: "2.jpg", views: 73},
			{url: "3.jpg", views: 34}
		];

		let downloaded_images = [
			{url: "1.jpg", views: 3},
			{url: "2.jpg", views: 5},
			{url: "3.jpg", views: 7},
			{url: "4.jpg", views: 3},
		];

		let other_images = [
			{url: "1.jpg", views: 0},
			{url: "2.jpg", views: 3},
			{url: "3.jpg", views: 0},
			{url: "4.jpg", views: 0},
			{url: "5.jpg", views: 1},
			{url: "1.jpg", views: 0},
		];

		return (
			<div className="App">
				<FriendsBar sel={this.state.selscreen === 0 ? this.state.seluser : ""} setUser={this.setUser} />
				<Main>
					<nav>
						<span className={s === 1 ? "selected" : ""} onClick={() => this.setScreen(1)}>My Images</span>
						<span className={s === 2 ? "selected" : ""} onClick={() => this.setScreen(2)}>Downloaded Images</span>
						<span className={s === 3 ? "selected" : ""} onClick={() => this.setScreen(3)}>Requests <div className="notification">5</div></span>
					</nav>
					{s === 0 && <OtherImages gallery={other_images} />}
					{s === 1 && <MyImages gallery={my_images} />}
					{s === 2 && <DownloadedImages gallery={downloaded_images} />}
					{s === 3 && <RequestList />}
				</Main>
			</div>
		);
	}
}

export default App;
