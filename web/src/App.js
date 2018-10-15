import React, { Component } from 'react';
import Main from "./sections/Main.js"
import MyImages from "./sections/MyImages.js"
import OtherImages from "./sections/OtherImages"
import DownloadedImages from "./sections/DownloadedImages"
import RequestList from "./sections/RequestList.js"
import Credentials from "./sections/Credentials.js"
import FriendsBar from "./sections/FriendsBar.js"
import "./css/main.css"

class App extends Component {
	constructor(props) {
		super(props);

		this.state = {
			selscreen: 1,
			my_images: [],
			my_images_loading: true,
			download_images: [],
			download_images_loading: true,
			other_images: [],
			other_images_loading: true,
			user_list: [],
			user_list_loading: true
		}
	}

	componentWillMount() {
		setTimeout(this.fetchMyImages, 1000);
		setTimeout(this.fetchDownloadedImages, 1000);
		setTimeout(this.fetchUserList, 400);
	}

	fetchMyImages = () => {
		fetch("/server/my.json").then((e) => e.json())
		.then((e) => {
			this.setState({my_images_loading: false, my_images: e});
		});
	}

	fetchOtherImages = () => {
		fetch("/server/other.json").then((e) => e.json())
		.then((e) => {
			this.setState({other_images_loading: false, other_images: e});
		});
	}

	fetchDownloadedImages = () => {
		fetch("/server/downloaded.json").then((e) => e.json())
		.then((e) => {
			this.setState({download_images_loading: false, download_images: e});
		});
	}

	fetchUserList = () => {
		fetch("/server/userlist.json").then((e) => e.json())
		.then((e) => {
			this.setState({user_list_loading: false, user_list: e});
		});
	}

	setUser = (u) => {
		if (this.state.seluser !== u) {
			// If we're looking at a different user's images, display them.
			this.setState({selscreen: 0, seluser: u});
			this.setState({other_images_loading: true});
			setTimeout(this.fetchOtherImages, 1000);
		}
		else {
			// If not just load the correct subscreen
			this.setState({selscreen: 0});
		}
	}

	setScreen = (i) => {
		this.setState({selscreen: i});
	}

	setUserInfo = (o) => {
		alert(o);
	}

	handleLogOut = () => {
		this.credentials.logOut();
	}

	render() {
		let s = this.state.selscreen;
			
		return (
			<div className="App">
				<FriendsBar sel={this.state.selscreen === 0 ? this.state.seluser : ""} setUser={this.setUser} loading={this.state.user_list_loading} list={this.state.user_list} />
				<Credentials ref={instance => { this.credentials = instance; }} setUserInfo={this.setUserInfo} />
				<main>
					<nav>
						<span className={s === 1 ? "selected" : ""} onClick={() => this.setScreen(1)}>My Images</span>
						<span className={s === 2 ? "selected" : ""} onClick={() => this.setScreen(2)}>Downloaded Images</span>
						<span className={s === 3 ? "selected" : ""} onClick={() => this.setScreen(3)}>Requests <div className="notification">5</div></span>
						<span className="signout-btn" onClick={this.handleLogOut}>Sign Out</span>
					</nav>
					{s === 0 && <OtherImages loading={this.state.other_images_loading} gallery={this.state.other_images} />}
					{s === 1 && <MyImages loading={this.state.my_images_loading} gallery={this.state.my_images} />}
					{s === 2 && <DownloadedImages loading={this.state.download_images_loading} gallery={this.state.download_images} />}
					{s === 3 && <RequestList />}
				</main>
			</div>
		);
	}
}

export default App;